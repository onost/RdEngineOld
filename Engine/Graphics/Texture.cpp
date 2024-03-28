#include "Texture.h"
#include "Core/GraphicsEngine.h"
#include "Core/GraphicsCommon.h"
#include "Helper/Helper.h"
#include "Helper/MyAssert.h"
#include <DirectXTex/d3dx12.h>
#include <format>

const std::string Texture::kTexturePath = "Assets/Texture/";

Texture::Texture()
	: mFilePath()
	, mBuff(nullptr)
	, mDescHandle(nullptr)
	, mWidth(0)
	, mHeight(0)
{

}

Texture::~Texture()
{
	// デスクリプタハンドルを解放
	gGraphicsEngine->GetSrvHeap().Free(mDescHandle);
}

bool Texture::Create(const std::string& filePath)
{
	mFilePath = filePath;
	// ワイド文字へ変換
	std::wstring wFilePath = Helper::ConvertToWstr(filePath);

	// ファイルを読み込む
	DirectX::ScratchImage scratchImage = {};
	HRESULT hr = DirectX::LoadFromWICFile(
		wFilePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, scratchImage);
	if (FAILED(hr))
	{
		return false;
	}

	// ミップマップを生成
	DirectX::ScratchImage mipChain = {};
	hr = GenerateMipMaps(
		scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipChain);
	if (FAILED(hr))
	{
		return false;
	}

	DirectX::TexMetadata metadata = mipChain.GetMetadata();
	mWidth = uint32_t(metadata.width);
	mHeight = uint32_t(metadata.height);
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.DepthOrArraySize = UINT16(metadata.arraySize);
	desc.MipLevels = UINT16(metadata.mipLevels);
	desc.Format = metadata.format;
	desc.SampleDesc.Count = 1;
	auto device = gGraphicsEngine->GetDevice();
	// テクスチャバッファを作成
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
		IID_PPV_ARGS(&mBuff));
	if (FAILED(hr))
	{
		return false;
	}

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), subresource);
	D3D12_RESOURCE_DESC intermediateDesc = {};
	intermediateDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	intermediateDesc.Width = GetRequiredIntermediateSize(mBuff.Get(), 0, UINT(subresource.size()));
	intermediateDesc.Height = 1;
	intermediateDesc.DepthOrArraySize = 1;
	intermediateDesc.MipLevels = 1;
	intermediateDesc.SampleDesc.Count = 1;
	intermediateDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 中間リソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
	hr = device->CreateCommittedResource(
		&GraphicsCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &intermediateDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&intermediateResource));
	if (FAILED(hr))
	{
		return false;
	}

	// テクスチャバッファへ転送
	auto cmdList = gGraphicsEngine->GetCmdList();
	UpdateSubresources(
		cmdList, mBuff.Get(), intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBuff.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmdList->ResourceBarrier(1, &barrier);
	gGraphicsEngine->ExecuteCommand();
	gGraphicsEngine->WaitGpu();

	// シェーダリソースビューを作成
	CreateSrv(DirectX::MakeSRGB(metadata.format), uint32_t(metadata.mipLevels));

	return true;
}

void Texture::CreateFromBuff(Microsoft::WRL::ComPtr<ID3D12Resource> buff)
{
	mBuff = buff;
	auto desc = mBuff->GetDesc();
	mWidth = uint32_t(desc.Width);
	mHeight = desc.Height;

	// シェーダリソースビューを作成
	CreateSrv(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
}

void Texture::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParam)
{
	MY_ASSERT(cmdList);
	cmdList->SetGraphicsRootDescriptorTable(rootParam, mDescHandle->mGpuHandle);
}

void Texture::CreateSrv(DXGI_FORMAT format, uint32_t mipLevels)
{
	// デスクリプタハンドルを割り当て
	mDescHandle = gGraphicsEngine->GetSrvHeap().Alloc();
	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = mipLevels;
	gGraphicsEngine->GetDevice()->CreateShaderResourceView(mBuff.Get(), &srvDesc, mDescHandle->mCpuHandle);
}
