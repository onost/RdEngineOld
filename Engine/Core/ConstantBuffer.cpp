#include "ConstantBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

ConstantBuffer::ConstantBuffer()
	: mBuff(nullptr)
	, mSize(0)
	, mData(nullptr)
{

}

void ConstantBuffer::Create(uint32_t size, void* initData)
{
	mSize = size;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = (mSize + 0xFF) & ~0xFF;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 定数バッファを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&mBuff));
	MY_ASSERT(SUCCEEDED(hr));

	mBuff->Map(0, nullptr, &mData);
	// 初期化用のデータがあればコピー
	if (initData)
	{
		Copy(initData);
	}
}

void ConstantBuffer::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParam)
{
	MY_ASSERT(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(rootParam, mBuff->GetGPUVirtualAddress());
}

void ConstantBuffer::Copy(void* data)
{
	memcpy(mData, data, mSize);
}
