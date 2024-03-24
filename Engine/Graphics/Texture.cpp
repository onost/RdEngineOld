#include "Texture.h"
#include "Core/GraphicsEngine.h"
#include "Core/GraphicsCommon.h"
#include "Helper/Helper.h"
#include "Helper/MyAssert.h"
#include <DirectXTex/d3dx12.h>
#include <format>

const std::string Texture::kTexturePath = "Assets/Texture/";

bool Texture::Create(const std::string& filePath)
{
	mPath = filePath;
	//Helper::WriteToConsole(std::format("Create: \"{}\"\n", mPath.c_str()));

	// テクスチャを読み込む
	DirectX::ScratchImage scratchImage = {};
	HRESULT hr = DirectX::LoadFromWICFile(
		Helper::ConvertToWstr(mPath).c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, scratchImage);
	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_INVALID_NAME))
	{
		return false;
	}
	MY_ASSERT(SUCCEEDED(hr));
	// ミップマップを作成
	DirectX::ScratchImage mipImage = {};
	hr = GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipImage);
	MY_ASSERT(SUCCEEDED(hr));

	DirectX::TexMetadata metadata = mipImage.GetMetadata();
	mDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
	mDesc.Width = metadata.width;
	mDesc.Height = UINT(metadata.height);
	mDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	mDesc.MipLevels = UINT16(metadata.mipLevels);
	mDesc.Format = metadata.format;
	mDesc.SampleDesc.Count = 1;
	// リソースを作成
	auto device = gGraphicsEngine->GetDevice();
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &mDesc, D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr, IID_PPV_ARGS(mResource.ReleaseAndGetAddressOf()));
	MY_ASSERT(SUCCEEDED(hr));

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, mipImage.GetImages(), mipImage.GetImageCount(), mipImage.GetMetadata(), subresource);
	auto intermediateSize = GetRequiredIntermediateSize(mResource.Get(), 0, UINT(subresource.size()));
	D3D12_RESOURCE_DESC intermediateDesc = {};
	intermediateDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	intermediateDesc.Width = intermediateSize;
	intermediateDesc.Height = 1;
	intermediateDesc.DepthOrArraySize = 1;
	intermediateDesc.MipLevels = 1;
	intermediateDesc.SampleDesc.Count = 1;
	intermediateDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 中間リソースを作成
	ID3D12Resource* intermediateResource;
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &intermediateDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&intermediateResource));
	MY_ASSERT(SUCCEEDED(hr));
	auto cmdList = gGraphicsEngine->GetCmdList();
	UpdateSubresources(cmdList, mResource.Get(), intermediateResource, 0, 0, UINT(subresource.size()), subresource.data());
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mResource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmdList->ResourceBarrier(1, &barrier);
	gGraphicsEngine->ExecuteCommand();
	gGraphicsEngine->WaitGpu();
	intermediateResource->Release();

	// シェーダリソースビューを作成
	metadata.format = DirectX::MakeSRGB(metadata.format);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata.format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	auto handle = gGraphicsEngine->GetSrvHeap().Alloc();
	device->CreateShaderResourceView(mResource.Get(), &srvDesc, handle->mCpuHandle);
	mDescHandle = handle->mGpuHandle;

	return true;
}

// リソースから作成
void Texture::Create(ID3D12Resource* resource)
{
	mResource.Attach(resource);
	mDesc = mResource->GetDesc();

	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	auto handle = gGraphicsEngine->GetSrvHeap().Alloc();
	gGraphicsEngine->GetDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, handle->mCpuHandle);
	mDescHandle = handle->mGpuHandle;
}

void Texture::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx)
{
	cmdList->SetGraphicsRootDescriptorTable(rootParamIdx, mDescHandle);
}
