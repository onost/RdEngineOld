#pragma once
#include "DescriptorHeap.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();
	void Create(uint32_t size, uint32_t elemCount, void* initData = nullptr);
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
	uint32_t mElemCount;
	// データへのポインタ
	void* mData;
	// デスクリプタハンドル
	DescriptorHandle* mDescHandle;
};
