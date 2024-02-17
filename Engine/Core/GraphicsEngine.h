#pragma once
#include "DescriptorHeap.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <memory>
#include <wrl.h>

class Window;

/*struct LeakChecker
{
public:
	~LeakChecker()
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};*/

class GraphicsEngine
{
public:
	void Initialize(Window* window);
	void Terminate();

	// レンダリング前
	void PreRendering();
	// レンダリング後
	void PostRendering();

	// コマンドリストを実行
	void ExecuteCommand();
	// 実行完了を待つ
	void WaitGpu();

	// SRVデスクリプタヒープ
	void SetSrvHeap();

	ID3D12Device* GetDevice() const { return mDevice.Get(); }
	ID3D12GraphicsCommandList* GetCmdList() const { return mCmdList.Get(); }
	DescriptorHeap& GetSrvHeap() { return mSrvHeap; }

private:
	void CreateDevice();
	void CreateCommand();
	void CreateSwapChain(Window* window);
	void CreateDescHeaps();
	void CreateRtv();
	void CreateDsv();
	void CreateFence();

private:
	//LeakChecker mLeakChecker;
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
	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandles[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	D3D12_CPU_DESCRIPTOR_HANDLE mDsvHandle;
	UINT64 mFenceVal;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent;
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;
	UINT mBackBuffIdx;
};

extern std::unique_ptr<GraphicsEngine> gGraphicsEngine;
