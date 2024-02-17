#pragma once
#include "Color.h"
#include "Graphics/Texture.h"
#include "Window.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class Texture;

// レンダーターゲット
class RenderTarget
{
public:
	void Create(uint32_t width = Window::kWidth, uint32_t height = Window::kHeight);

	// レンダリング前
	void PreRendering(ID3D12GraphicsCommandList* cmdList);
	// レンダリング後
	void PostRendering(ID3D12GraphicsCommandList* cmdList);

	Texture* GetTexture() const { return mTexture.get(); }

private:
	std::unique_ptr<Texture> mTexture;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;
	Color mClearColor;
};
