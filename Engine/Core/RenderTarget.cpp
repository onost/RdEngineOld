#include "RenderTarget.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

RenderTarget::RenderTarget()
	: mCmdList(nullptr)
	, mRenderTarget(nullptr)
	, mRtvHandle(nullptr)
	, mDepthBuff(nullptr)
	, mDsvHandle(nullptr)
	, mViewport()
	, mScissor()
	, mClearColor(Color::kBlue)
{

}

RenderTarget::~RenderTarget()
{
	// デスクリプタハンドルを解放
	gGraphicsEngine->GetRtvHeap().Free(mRtvHandle);
	gGraphicsEngine->GetDsvHeap().Free(mDsvHandle);
}

void RenderTarget::Create(uint32_t width, uint32_t height)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	memcpy(clearVal.Color, &mClearColor.r, sizeof(float) * 4);
	// レンダーターゲットを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff = nullptr;
	auto device = gGraphicsEngine->GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearVal,
		IID_PPV_ARGS(&texBuff));
	MY_ASSERT(SUCCEEDED(hr));
	// テクスチャを作成
	mRenderTarget = std::make_shared<Texture>();
	mRenderTarget->CreateFromBuff(texBuff);

	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	clearVal.Format = DXGI_FORMAT_D32_FLOAT;
	clearVal.DepthStencil.Depth = 1.0f;
	// 深度バッファを作成
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal,
		IID_PPV_ARGS(&mDepthBuff));
	MY_ASSERT(SUCCEEDED(hr));

	// デスクリプタハンドルを割り当て
	mRtvHandle = gGraphicsEngine->GetRtvHeap().Alloc();
	mDsvHandle = gGraphicsEngine->GetDsvHeap().Alloc();
	// レンダーターゲットビューを作成
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(texBuff.Get(), &rtvDesc, mRtvHandle->mCpuHandle);
	// 深度ステンシルビューを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mDsvHandle->mCpuHandle);

	// ビューポートとシザー矩形
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = FLOAT(width);
	mViewport.Height = FLOAT(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissor.left = 0;
	mScissor.top = 0;
	mScissor.right = width;
	mScissor.bottom = height;
}

// レンダリング前処理
void RenderTarget::PreRender(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(!mCmdList);
	mCmdList = cmdList;

	// シェーダリソースからレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mRenderTarget->GetBuff();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットをセット
	auto& rtvHandle = mRtvHandle->mCpuHandle;
	auto& dsvHandle = mDsvHandle->mCpuHandle;
	mCmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// クリア
	mCmdList->ClearRenderTargetView(rtvHandle, &mClearColor.r, 0, nullptr);
	mCmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートとシザー矩形
	mCmdList->RSSetViewports(1, &mViewport);
	mCmdList->RSSetScissorRects(1, &mScissor);
}

// レンダリング後処理
void RenderTarget::PostRender()
{
	MY_ASSERT(mCmdList);

	// レンダーターゲットからシェーダリソースへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mRenderTarget->GetBuff();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	mCmdList->ResourceBarrier(1, &barrier);

	mCmdList = nullptr;
}
