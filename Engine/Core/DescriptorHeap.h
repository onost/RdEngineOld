#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// デスクリプタハンドル
struct DescriptorHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle;
};

// デスクリプタヒープ
class DescriptorHeap
{
public:
	void Create(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescs, bool shaderVisible);
	DescriptorHandle Allocate();
	ID3D12DescriptorHeap* Get() const { return mDescHeap.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescHeap;
	uint32_t mNumDescs;
	uint32_t mDescSize;
	DescriptorHandle mStartHandle;
	uint32_t mIndex;
};
