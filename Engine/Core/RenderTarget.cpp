#include "RenderTarget.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"
//#include "Window.h"

void RenderTarget::Create(uint32_t width, uint32_t height)
{
	mClearColor = Color::kBlue;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE clearCol = {};
	clearCol.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearCol.Color[0] = mClearColor.r;
	clearCol.Color[1] = mClearColor.g;
	clearCol.Color[2] = mClearColor.b;
	clearCol.Color[3] = mClearColor.a;
	ID3D12Resource* texture = nullptr;
	auto device = gGraphicsEngine->GetDevice();
	// テクスチャバッファ
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearCol, IID_PPV_ARGS(&texture));
	MyAssert(SUCCEEDED(hr));
	mTexture = std::make_unique<Texture>();
	mTexture->Create(texture);

	// ==================================================
	// レンダーターゲット
	// ==================================================

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.NumDescriptors = 1;
	hr = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(mRtvHeap.ReleaseAndGetAddressOf()));
	MyAssert(SUCCEEDED(hr));
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(mTexture->GetResource(), &rtvDesc, mRtvHeap->GetCPUDescriptorHandleForHeapStart());

	// ==================================================
	// 深度バッファ
	// ==================================================

	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hr = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(mDsvHeap.ReleaseAndGetAddressOf()));
	MyAssert(SUCCEEDED(hr));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue, IID_PPV_ARGS(mDepthBuff.ReleaseAndGetAddressOf()));
	MyAssert(SUCCEEDED(hr));
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

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

// レンダリング前
void RenderTarget::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mTexture->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList->ResourceBarrier(1, &barrier);
	auto rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvHandle = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	cmdList->ClearRenderTargetView(rtvHandle, &mClearColor.r, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	cmdList->RSSetViewports(1, &mViewport);
	cmdList->RSSetScissorRects(1, &mScissor);
}

// レンダリング後
void RenderTarget::PostRendering(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mTexture->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	cmdList->ResourceBarrier(1, &barrier);
}
