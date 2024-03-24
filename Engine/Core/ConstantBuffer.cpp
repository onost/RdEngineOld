#include "ConstantBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

void ConstantBuffer::Create(uint32_t size, void* initData)
{
	mSize = size;
	// 256アライメントを計算
	uint32_t alignedSize = (mSize + 0xFF) & ~0xFF;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = alignedSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 定数バッファを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&mResource));
	MY_ASSERT(SUCCEEDED(hr));
	// マップ
	mResource->Map(0, nullptr, &mData);
	if (initData)
	{
		Copy(initData);
	}
}

void ConstantBuffer::Bind(
	ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx)
{
	cmdList->SetGraphicsRootConstantBufferView(
		rootParamIdx, mResource->GetGPUVirtualAddress());
}

void ConstantBuffer::Copy(void* data)
{
	memcpy(mData, data, mSize);
}
