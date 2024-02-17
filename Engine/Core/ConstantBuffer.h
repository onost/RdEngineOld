#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ
class ConstantBuffer
{
public:
	void Create(uint32_t size, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx);
	// 定数データをコピー
	void Copy(void* data);
	template <class T>
	void Copy(T& data)
	{
		Copy(&data);
	}

	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
	uint32_t mSize;
	void* mData;
};
