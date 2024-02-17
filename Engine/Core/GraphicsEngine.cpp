#include "GraphicsEngine.h"
#include "GraphicsCommon.h"
#include "Helper/MyAssert.h"
#include "Shader.h"
#include "Window.h"
#include <format>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")

void GraphicsEngine::Initialize(Window* window)
{
	MyAssert(window);

	CreateDevice();
	CreateCommand();
	CreateSwapChain(window);
	CreateDescHeaps();
	CreateRtv();
	CreateDsv();
	CreateFence();
	// DXCを初期化
	Shader::Initialize();

	// ビューポート
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = FLOAT(Window::kWidth);
	mViewport.Height = FLOAT(Window::kHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// シザー矩形
	mScissor.left = 0;
	mScissor.top = 0;
	mScissor.right = Window::kWidth;
	mScissor.bottom = Window::kHeight;
}

void GraphicsEngine::Terminate()
{
	if (mFenceEvent)
	{
		CloseHandle(mFenceEvent);
		mFenceEvent = nullptr;
	}
}

void GraphicsEngine::PreRendering()
{
	// バックバッファのインデックスを取得
	mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();
	// リソースバリア
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCmdList->ResourceBarrier(1, &barrier);
	// RTVとDSVをセット
	mCmdList->OMSetRenderTargets(1, &mRtvHandles[mBackBuffIdx], false, &mDsvHandle);
	// クリア
	float clearColor[4] = { 0.0f,0.0f,1.0f,0.0f };
	mCmdList->ClearRenderTargetView(mRtvHandles[mBackBuffIdx], clearColor, 0, nullptr);
	mCmdList->ClearDepthStencilView(mDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// ビューポートとシザー矩形
	mCmdList->RSSetViewports(1, &mViewport);
	mCmdList->RSSetScissorRects(1, &mScissor);
	// SRVデスクリプタヒープ
	SetSrvHeap();
}

void GraphicsEngine::PostRendering()
{
	// リソースバリア
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCmdList->ResourceBarrier(1, &barrier);

	// コマンドリストを実行
	ExecuteCommand();
	mSwapChain->Present(1, 0);
	WaitGpu();
}

void GraphicsEngine::ExecuteCommand()
{
	mCmdList->Close();
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(1, cmdLists);
}

void GraphicsEngine::WaitGpu()
{
	++mFenceVal;
	mCmdQueue->Signal(mFence.Get(), mFenceVal);
	if (mFence->GetCompletedValue() < mFenceVal)
	{
		mFence->SetEventOnCompletion(mFenceVal, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}

	mCmdAllocator->Reset();
	mCmdList->Reset(mCmdAllocator.Get(), nullptr);
}

void GraphicsEngine::SetSrvHeap()
{
	ID3D12DescriptorHeap* descHeaps[] = { mSrvHeap.Get() };
	mCmdList->SetDescriptorHeaps(1, descHeaps);
}

void GraphicsEngine::CreateDevice()
{
#ifdef _DEBUG
	// デバッグレイヤーを有効にする
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);
	}
#endif
	// DXGIファクトリーを作成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));
	MyAssert(SUCCEEDED(hr));
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;
	// パフォーマンス順でアダプタを取得
	for (uint32_t i = 0;
		mFactory->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		DXGI_ADAPTER_DESC3 desc = {};
		hr = useAdapter->GetDesc3(&desc);
		if (SUCCEEDED(hr))
		{
			if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
			{
				Helper::WriteToConsole(
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
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&mDevice));
		if (SUCCEEDED(hr))
		{
			Helper::WriteToConsole(std::format("Feature level: {}\n", str[i]));
			break;
		}
	}
	MyAssert(mDevice != nullptr);
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

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

void GraphicsEngine::CreateCommand()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	// コマンドキューを作成
	HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCmdQueue));
	MyAssert(SUCCEEDED(hr));
	// コマンドアロケータを作成
	hr = mDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllocator));
	MyAssert(SUCCEEDED(hr));
	// コマンドリストを作成
	hr = mDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
	MyAssert(SUCCEEDED(hr));
}

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
	// スワップチェインを作成
	[[maybe_unused]] HRESULT hr = mFactory->CreateSwapChainForHwnd(
		mCmdQueue.Get(), window->GetHWnd(), &desc, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf()));
	MyAssert(SUCCEEDED(hr));
}

void GraphicsEngine::CreateDescHeaps()
{
	// デスクリプタヒープを作成
	mRtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	mDsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	mSrvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
}

void GraphicsEngine::CreateRtv()
{
	for (uint32_t i = 0; i < 2; ++i)
	{
		// バックバッファを取得
		[[maybe_unused]] HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffs[i]));
		MyAssert(SUCCEEDED(hr));
		mRtvHandles[i] = mRtvHeap.Allocate().mCpuHandle;
		// レンダーターゲットビューを作成
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		mDevice->CreateRenderTargetView(mBackBuffs[i].Get(), &rtvDesc, mRtvHandles[i]);
	}
}

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
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	// 深度バッファを作成
	[[maybe_unused]] HRESULT hr = mDevice->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue, IID_PPV_ARGS(&mDepthBuff));
	MyAssert(SUCCEEDED(hr));
	mDsvHandle = mDsvHeap.Allocate().mCpuHandle;
	// 深度ステンシルビューを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	mDevice->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mDsvHandle);
}

void GraphicsEngine::CreateFence()
{
	// フェンスを作成
	[[maybe_unused]] HRESULT hr = mDevice->CreateFence(
		mFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	MyAssert(SUCCEEDED(hr));
	mFenceEvent = CreateEvent(nullptr, false, false, nullptr);
	MyAssert(mFenceEvent);
}

std::unique_ptr<GraphicsEngine> gGraphicsEngine = nullptr;
