#pragma once
#include <d3d12.h>
#include <DirectXTex/DirectXTex.h>
#include <string>
#include <wrl.h>

// テクスチャ
class Texture
{
public:
	static const std::string kTexturePath;

	bool Create(const std::string& filePath);
	void Create(ID3D12Resource* resource);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx);

	const std::string& GetPath() const { return mPath; }
	uint32_t GetWidth() const { return static_cast<uint32_t>(mDesc.Width); }
	uint32_t GetHeight() const { return mDesc.Height; }
	ID3D12Resource* GetResource() const { return mResource.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetDescHandle() const { return mDescHandle; }

private:
	std::string mPath;
	D3D12_RESOURCE_DESC mDesc;
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
	D3D12_GPU_DESCRIPTOR_HANDLE mDescHandle;
};
