#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class ConstantBuffer
{
public:
	ConstantBuffer();
	void Create(uint32_t size, void* initData = nullptr);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParam);

	// VRAMへコピー
	void Copy(void* data);
	template <typename T>
	void Copy(T& data)
	{
		Copy(&data);
	}

	void* GetData() const { return mData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	uint32_t mSize;
	// データへのポインタ
	void* mData;
};
