#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// 頂点バッファ
class VertexBuffer
{
public:
	void Create(uint32_t size, uint32_t stride, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList);
	// 頂点データをコピー
	void Copy(void* src);

	const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return mView; }
	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
	D3D12_VERTEX_BUFFER_VIEW mView;
	void* mData;
};
