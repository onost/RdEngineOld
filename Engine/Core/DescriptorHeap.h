#pragma once
#include "Helper/Pool.h"
#include <d3d12.h>
#include <wrl.h>

struct DescriptorHandle
{
	// CPUデスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle;
	// GPUデスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle;
};

class DescriptorHeap
{
public:
	DescriptorHeap();

	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescs, bool isShaderVisible);
	// デスクリプタハンドルを割り当て
	DescriptorHandle* Alloc();
	// デスクリプタハンドルを解放
	void Free(DescriptorHandle*& descHandle);

	ID3D12DescriptorHeap* GetHeap() const { return mDescriptorHeap.Get(); }

private:
	D3D12_DESCRIPTOR_HEAP_DESC mDesc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
	// デスクリプタハンドル管理用
	Pool<DescriptorHandle> mDescriptorPool;
	// デスクリプタのインクリメントサイズ
	uint32_t mIncrementSize;
};
