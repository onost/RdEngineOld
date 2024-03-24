#include "StructuredBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include "Helper/MyAssert.h"

void StructuredBuffer::Create(uint32_t size, uint32_t elementCount, void* initData)
{
	mSize = size;
	mElementCount = elementCount;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = mSize * mElementCount;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	auto device = gGraphicsEngine->GetDevice();
	// 構造化バッファを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&mResource));
	MY_ASSERT(SUCCEEDED(hr));
	// マップ
	mResource->Map(0, nullptr, &mData);
	if (initData)
	{
		Update(initData);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = mElementCount;
	srvDesc.Buffer.StructureByteStride = mSize;
	auto handle = gGraphicsEngine->GetSrvHeap().Alloc();
	// シェーダリソースビューを作成
	device->CreateShaderResourceView(mResource.Get(), &srvDesc, handle->mCpuHandle);
	mDescHandle = handle->mGpuHandle;
}

void StructuredBuffer::Bind(
	ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx)
{
	cmdList->SetGraphicsRootDescriptorTable(rootParamIdx, mDescHandle);
}

void StructuredBuffer::Update(void* data)
{
	memcpy(mData, data, mSize * mElementCount);
}
