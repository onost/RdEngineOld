#pragma once
#include "DescriptorHeap.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <memory>
#include <string>
#include <wrl.h>

class Window;

// DirectX
class GraphicsEngine
{
public:
	GraphicsEngine();

	void Initialize(Window* window);
	void Terminate();

	// SRV用ヒープをバインド
	void SetSrvHeap();

	// レンダリング前後処理
	void PreRender();
	void PostRender();

	// コマンドを実行
	void ExecuteCommand();
	// 実行の完了を待つ
	void WaitGpu();

	// シェーダをコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::string& filePath, const std::string& profile);

	// アクセッサ
	ID3D12Device* GetDevice() const { return mDevice.Get(); }
	ID3D12GraphicsCommandList* GetCmdList() const { return mCmdList.Get(); }
	DescriptorHeap& GetRtvHeap() { return mRtvHeap; }
	DescriptorHeap& GetDsvHeap() { return mDsvHeap; }
	DescriptorHeap& GetSrvHeap() { return mSrvHeap; }

private:
	void CreateDevice();
	void CreateCommand();
	void CreateSwapChain(Window* window);
	void CreateDescriptorHeaps();
	void CreateRtv();
	void CreateDsv();
	void CreateFence();
	void InitializeDxc();

private:
	Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	DescriptorHeap mRtvHeap;
	DescriptorHeap mDsvHeap;
	DescriptorHeap mSrvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffs[2];
	DescriptorHandle* mRtvHandles[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mDsvHandle;
	UINT64 mFenceVal;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;
	UINT mBackBuffIdx;

	// DirectX Shader Compiler
	Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
};

extern std::shared_ptr<GraphicsEngine> gGraphicsEngine;
