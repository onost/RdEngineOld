#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// インデックスバッファ
class IndexBuffer
{
public:
	void Create(uint32_t size, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList);
	// インデックスデータをコピー
	void Copy(void* data);

	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
	D3D12_INDEX_BUFFER_VIEW mView;
	void* mData;
};
