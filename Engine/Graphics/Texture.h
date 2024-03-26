#pragma once
#include "Core/DescriptorHeap.h"
#include <d3d12.h>
#include <DirectXTex/DirectXTex.h>
#include <string>
#include <wrl.h>

class Texture
{
public:
	Texture();
	~Texture();
	bool Create(const std::string& filePath);
	void CreateFromBuff(Microsoft::WRL::ComPtr<ID3D12Resource> buff);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParam);

	// アクセッサ
	const std::string& GetFilePath() const { return mFilePath; }
	ID3D12Resource* GetBuff() const { return mBuff.Get(); }
	DescriptorHandle* GetDescHandle() const { return mDescHandle; }
	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }

private:
	// シェーダリソースビューを作成
	void CreateSrv(DXGI_FORMAT format, uint32_t mipLevels);

public:
	// テクスチャへのパス
	static const std::string kTexturePath;
private:
	std::string mFilePath;

	//D3D12_RESOURCE_DESC mDesc;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	// デスクリプタハンドル
	DescriptorHandle* mDescHandle;

	// テクスチャサイズ
	uint32_t mWidth;
	uint32_t mHeight;
};
