#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// 構造化バッファ
class StructuredBuffer
{
public:
	void Create(uint32_t size, uint32_t elementCount, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx);
	// 構造化データをコピー
	void Update(void* data);
	template <class T>
	void Update(T& data)
	{
		Update(&data);
	}

	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
	uint32_t mSize;
	uint32_t mElementCount;
	D3D12_GPU_DESCRIPTOR_HANDLE mDescHandle;
	void* mData;
};
