#include "IndexBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

IndexBuffer::IndexBuffer()
	: mBuff(nullptr)
	, mView()
	, mData(nullptr)
{

}

void IndexBuffer::Create(uint32_t size, void* initData)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// インデックスバッファを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&mBuff));
	MY_ASSERT(SUCCEEDED(hr));

	// インデックスバッファビュー
	mView.BufferLocation = mBuff->GetGPUVirtualAddress();
	mView.SizeInBytes = size;
	mView.Format = DXGI_FORMAT_R32_UINT;

	mBuff->Map(0, nullptr, &mData);
	// 初期化用のデータがあればコピー
	if (initData)
	{
		Copy(initData);
	}
}

void IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	cmdList->IASetIndexBuffer(&mView);
}

void IndexBuffer::Copy(void* data)
{
	memcpy(mData, data, mView.SizeInBytes);
}
