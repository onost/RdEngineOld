#include "DescriptorHeap.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

DescriptorHeap::DescriptorHeap()
	: mDesc()
	, mDescriptorHeap(nullptr)
	, mDescriptorPool()
	, mIncrementSize(0)
{

}

// デスクリプタヒープを作成
void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescs, bool isShaderVisible)
{
	mDesc.Type = type;
	mDesc.NumDescriptors = numDescs;
	mDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto device = gGraphicsEngine->GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateDescriptorHeap(&mDesc, IID_PPV_ARGS(&mDescriptorHeap));
	MY_ASSERT(SUCCEEDED(hr));
	// プールを初期化
	mDescriptorPool.Initialize(numDescs);
	// インクリメントサイズを取得
	mIncrementSize = device->GetDescriptorHandleIncrementSize(type);
}

// デスクリプタハンドルを割り当て
DescriptorHandle* DescriptorHeap::Alloc()
{
	auto initFunc = [&](uint32_t index, DescriptorHandle& descHandle)
		{
			// CPUハンドルを計算
			descHandle.mCpuHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			descHandle.mCpuHandle.ptr += mIncrementSize * index;
			// GPUハンドルを計算
			if (mDesc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			{
				descHandle.mGpuHandle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descHandle.mGpuHandle.ptr += mIncrementSize * index;
			}
		};
	return mDescriptorPool.Alloc(initFunc);
}

// デスクリプタハンドルを解放
void DescriptorHeap::Free(DescriptorHandle*& descHandle)
{
	if (descHandle)
	{
		mDescriptorPool.Free(descHandle);
		descHandle = nullptr;
	}
}
