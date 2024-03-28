#pragma once
#include "Color.h"
#include "DescriptorHeap.h"
#include "Graphics/Texture.h"
#include "Window.h"
#include <memory>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();
	void Create(uint32_t width = Window::kWidth, uint32_t height = Window::kHeight);
	// レンダリング前後処理
	void PreRender(ID3D12GraphicsCommandList* cmdList);
	void PostRender();

	std::shared_ptr<Texture> GetRenderTarget() const { return mRenderTarget; }

private:
	ID3D12GraphicsCommandList* mCmdList;

	// レンダーターゲット
	std::shared_ptr<Texture> mRenderTarget;
	DescriptorHandle* mRtvHandle;
	// 深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mDsvHandle;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;
	Color mClearColor;
};
