#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class VertexBuffer
{
public:
	VertexBuffer();
	void Create(uint32_t size, uint32_t stride, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList);

	// VRAMへコピー
	void Copy(void* data);

	const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return mView; }
	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	D3D12_VERTEX_BUFFER_VIEW mView;
	// データへのポインタ
	void* mData;
};
