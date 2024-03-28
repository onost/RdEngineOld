#include "GraphicsEngine.h"
#include "GraphicsCommon.h"
#include "Helper/MyAssert.h"
#include "Window.h"
#include <format>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxcompiler.lib")

std::shared_ptr<GraphicsEngine> gGraphicsEngine = nullptr;

GraphicsEngine::GraphicsEngine()
	: mFactory(nullptr)
	, mDevice(nullptr)
	, mCmdQueue(nullptr)
	, mCmdAllocator(nullptr)
	, mCmdList(nullptr)
	, mSwapChain(nullptr)
	, mRtvHeap()
	, mDsvHeap()
	, mSrvHeap()
	, mBackBuffs{ nullptr,nullptr }
	, mRtvHandles{ nullptr,nullptr }
	, mDepthBuff(nullptr)
	, mDsvHandle(nullptr)
	, mFenceVal(0)
	, mFence(nullptr)
	, mFenceEvent(nullptr)
	, mViewport()
	, mScissor()
	, mBackBuffIdx(0)
	, mUtils(nullptr)
	, mCompiler(nullptr)
	, mIncludeHandler(nullptr)
{

}

void GraphicsEngine::Initialize(Window* window)
{
	MY_ASSERT(window);

	CreateDevice();
	CreateCommand();
	CreateSwapChain(window);
	CreateDescriptorHeaps();
	CreateRtv();
	CreateDsv();
	CreateFence();
	InitializeDxc();

	// ビューポートとシザー矩形
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = FLOAT(Window::kWidth);
	mViewport.Height = FLOAT(Window::kHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissor.left = 0;
	mScissor.top = 0;
	mScissor.right = Window::kWidth;
	mScissor.bottom = Window::kHeight;
}

void GraphicsEngine::Terminate()
{
	mRtvHeap.Free(mRtvHandles[0]);
	mRtvHeap.Free(mRtvHandles[1]);
	mDsvHeap.Free(mDsvHandle);
	if (mFenceEvent)
	{
		CloseHandle(mFenceEvent);
		mFenceEvent = nullptr;
	}
}

// SRV用ヒープをバインド
void GraphicsEngine::SetSrvHeap()
{
	ID3D12DescriptorHeap* descHeaps[] = { mSrvHeap.GetHeap() };
	mCmdList->SetDescriptorHeaps(1, descHeaps);
}

// レンダリング前処理
void GraphicsEngine::PreRender()
{
	mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();

	// 表示からレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットをセット
	auto& rtvHandle = mRtvHandles[mBackBuffIdx]->mCpuHandle;
	auto& dsvHandle = mDsvHandle->mCpuHandle;
	mCmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// クリア
	float clearColor[4] = { 0.0f,0.0f,1.0f,0.0f };
	mCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	mCmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートとシザー矩形
	mCmdList->RSSetViewports(1, &mViewport);
	mCmdList->RSSetScissorRects(1, &mScissor);

	SetSrvHeap();
}

// レンダリング後処理
void GraphicsEngine::PostRender()
{
	// レンダーターゲットから表示へ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCmdList->ResourceBarrier(1, &barrier);

	ExecuteCommand();
	mSwapChain->Present(1, 0);
	WaitGpu();
}

// コマンドを実行
void GraphicsEngine::ExecuteCommand()
{
	mCmdList->Close();
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(1, cmdLists);
}

// 実行の完了を待つ
void GraphicsEngine::WaitGpu()
{
	++mFenceVal;
	mCmdQueue->Signal(mFence.Get(), mFenceVal);
	if (mFence->GetCompletedValue() < mFenceVal)
	{
		mFence->SetEventOnCompletion(mFenceVal, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}

	// リセット
	mCmdAllocator->Reset();
	mCmdList->Reset(mCmdAllocator.Get(), nullptr);
}

// シェーダをコンパイル
Microsoft::WRL::ComPtr<IDxcBlob> GraphicsEngine::CompileShader(
	const std::string& filePath, const std::string& profile)
{
	// ワイド文字へ変換
	std::wstring wFilePath = Helper::ConvertToWstr(filePath);
	std::wstring wProfile = Helper::ConvertToWstr(profile);

	// ファイルを読み込む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> blobEncoding = nullptr;
	HRESULT hr = mUtils->LoadFile(wFilePath.c_str(), nullptr, &blobEncoding);
	if (FAILED(hr))
	{
		return nullptr;
	}

	// コンパイル
	DxcBuffer source = {};
	source.Ptr = blobEncoding->GetBufferPointer();
	source.Size = blobEncoding->GetBufferSize();
	source.Encoding = DXC_CP_UTF8;
	// コンパイルオプション
	LPCWSTR options[] =
	{
		wFilePath.c_str(),
		L"-E", L"main",
		L"-T", wProfile.c_str(),
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	Microsoft::WRL::ComPtr<IDxcResult> result = nullptr;
	hr = mCompiler->Compile(
		&source, options, _countof(options), mIncludeHandler.Get(), IID_PPV_ARGS(&result));
	if (FAILED(hr))
	{
		return nullptr;
	}

	Microsoft::WRL::ComPtr<IDxcBlobUtf8> error = nullptr;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr);
	if (error && error->GetStringLength() != 0)
	{
		// エラーの出力
		Helper::WriteToOutputWindow(error->GetStringPointer());
		return nullptr;
	}

	// シェーダオブジェクトを返す
	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr);
	if (FAILED(hr))
	{
		return nullptr;
	}
	return blob;
}

// デバイス
void GraphicsEngine::CreateDevice()
{
#ifdef _DEBUG
	// デバッグレイヤーを有効にする
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);
	}
#endif

	// DXGIファクトリーを作成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));
	MY_ASSERT(SUCCEEDED(hr));

	// パフォーマンス順でアダプタを取得
	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = nullptr;
	for (uint32_t i = 0;
		mFactory->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		DXGI_ADAPTER_DESC3 desc = {};
		hr = adapter->GetDesc3(&desc);
		if (SUCCEEDED(hr))
		{
			// ソフトウェアアダプタはダメ
			if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
			{
				Helper::WriteToOutputWindow(
					Helper::ConvertToStr(std::format(L"Use adapter: {}\n", desc.Description)));
				break;
			}
		}
	}

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	const char* str[] = { "12.2","12.1","12.0" };
	for (uint32_t i = 0; i < _countof(featureLevels); ++i)
	{
		// デバイスを作成
		hr = D3D12CreateDevice(adapter.Get(), featureLevels[i], IID_PPV_ARGS(&mDevice));
		if (SUCCEEDED(hr))
		{
			Helper::WriteToOutputWindow(std::format("Feature level: {}\n", str[i]));
			break;
		}
	}
	MY_ASSERT(mDevice);

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// 警告やエラーが出たら中断
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// どうしようもないエラーは無視
		// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_MESSAGE_ID ids[] = { D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(ids);
		filter.DenyList.pIDList = ids;
		infoQueue->PushStorageFilter(&filter);
	}
#endif

	GraphicsCommon::Initialize();
}

// コマンド
void GraphicsEngine::CreateCommand()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	// コマンドキューを作成
	HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCmdQueue));
	MY_ASSERT(SUCCEEDED(hr));
	// コマンドアロケータを作成
	hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllocator));
	MY_ASSERT(SUCCEEDED(hr));
	// コマンドリストを作成
	hr = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
	MY_ASSERT(SUCCEEDED(hr));
}

// スワップチェイン
void GraphicsEngine::CreateSwapChain(Window* window)
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = Window::kWidth;
	desc.Height = Window::kHeight;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	[[maybe_unused]] HRESULT hr = mFactory->CreateSwapChainForHwnd(
		mCmdQueue.Get(), window->GetHWnd(), &desc, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf()));
	MY_ASSERT(SUCCEEDED(hr));
}

// デスクリプタヒープ
void GraphicsEngine::CreateDescriptorHeaps()
{
	mRtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128, false);
	mDsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 128, false);
	mSrvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 512, true);
}

// レンダーターゲットビュー
void GraphicsEngine::CreateRtv()
{
	for (uint32_t i = 0; i < 2; ++i)
	{
		// バックバッファを取得
		[[maybe_unused]] HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffs[i]));
		MY_ASSERT(SUCCEEDED(hr));

		mRtvHandles[i] = mRtvHeap.Alloc();

		// レンダーターゲットビューを作成
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		mDevice->CreateRenderTargetView(mBackBuffs[i].Get(), &rtvDesc, mRtvHandles[i]->mCpuHandle);
	}
}

// 深度ステンシルビュー
void GraphicsEngine::CreateDsv()
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = Window::kWidth;
	desc.Height = Window::kHeight;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Format = DXGI_FORMAT_D32_FLOAT;
	clearVal.DepthStencil.Depth = 1.0f;
	// 深度バッファを作成
	[[maybe_unused]] HRESULT hr = mDevice->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal,
		IID_PPV_ARGS(&mDepthBuff));
	MY_ASSERT(SUCCEEDED(hr));

	mDsvHandle = mDsvHeap.Alloc();

	// 深度ステンシルビューを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	mDevice->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mDsvHandle->mCpuHandle);
}

// フェンス
void GraphicsEngine::CreateFence()
{
	[[maybe_unused]] HRESULT hr = mDevice->CreateFence(mFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	MY_ASSERT(SUCCEEDED(hr));
	mFenceEvent = CreateEvent(nullptr, false, false, nullptr);
	MY_ASSERT(mFenceEvent);
}

// DirectX Shader Compiler
void GraphicsEngine::InitializeDxc()
{
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mUtils));
	MY_ASSERT(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));
	MY_ASSERT(SUCCEEDED(hr));
	hr = mUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	MY_ASSERT(SUCCEEDED(hr));
}
