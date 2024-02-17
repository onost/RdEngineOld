#include "DescriptorHeap.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

void DescriptorHeap::Create(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescs, bool shaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = heapType;
	desc.NumDescriptors = numDescs;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto device = gGraphicsEngine->GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDescHeap));
	MyAssert(SUCCEEDED(hr));
	mNumDescs = numDescs;
	mDescSize = device->GetDescriptorHandleIncrementSize(heapType);
	mStartHandle.mCpuHandle = mDescHeap->GetCPUDescriptorHandleForHeapStart();
	if (shaderVisible)
	{
		mStartHandle.mGpuHandle = mDescHeap->GetGPUDescriptorHandleForHeapStart();
	}
}

DescriptorHandle DescriptorHeap::Allocate()
{
	MyAssert(mIndex < mNumDescs);
	DescriptorHandle handle = mStartHandle;
	handle.mCpuHandle.ptr += mDescSize * mIndex;
	handle.mGpuHandle.ptr += mDescSize * mIndex;
	++mIndex;
	return handle;
}
