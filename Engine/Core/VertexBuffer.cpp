#include "VertexBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

VertexBuffer::VertexBuffer()
	: mBuff(nullptr)
	, mView()
	, mData(nullptr)
{

}

void VertexBuffer::Create(uint32_t size, uint32_t stride, void* initData)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 頂点バッファを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&mBuff));
	MY_ASSERT(SUCCEEDED(hr));

	// 頂点バッファビュー
	mView.BufferLocation = mBuff->GetGPUVirtualAddress();
	mView.SizeInBytes = size;
	mView.StrideInBytes = stride;

	mBuff->Map(0, nullptr, &mData);
	// 初期化用のデータがあればコピー
	if (initData)
	{
		Copy(initData);
	}
}

void VertexBuffer::Bind(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	cmdList->IASetVertexBuffers(0, 1, &mView);
}

void VertexBuffer::Copy(void* data)
{
	memcpy(mData, data, mView.SizeInBytes);
}
