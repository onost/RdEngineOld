#include "StructuredBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

StructuredBuffer::StructuredBuffer()
	: mBuff(nullptr)
	, mSize(0)
	, mElemCount(0)
	, mData(nullptr)
	, mDescHandle(nullptr)
{

}

StructuredBuffer::~StructuredBuffer()
{
	// デスクリプタハンドルを解放
	gGraphicsEngine->GetSrvHeap().Free(mDescHandle);
}

void StructuredBuffer::Create(uint32_t size, uint32_t elemCount, void* initData)
{
	mSize = size;
	mElemCount = elemCount;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = mSize * mElemCount;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 構造化バッファを作成
	auto device = gGraphicsEngine->GetDevice();
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

	// デスクリプタハンドルを割り当て
	mDescHandle = gGraphicsEngine->GetSrvHeap().Alloc();
	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = mElemCount;
	srvDesc.Buffer.StructureByteStride = mSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(mBuff.Get(), &srvDesc, mDescHandle->mCpuHandle);
}

void StructuredBuffer::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParam)
{
	MY_ASSERT(cmdList);
	cmdList->SetGraphicsRootDescriptorTable(rootParam, mDescHandle->mGpuHandle);
}

void StructuredBuffer::Copy(void* data)
{
	memcpy(mData, data, mSize * mElemCount);
}
