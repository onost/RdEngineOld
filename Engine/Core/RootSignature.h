#pragma once
#include "Helper/MyAssert.h"
#include "RootParameter.h"
#include <memory>
#include <wrl.h>

class RootSignature
{
public:
	RootSignature(uint32_t numRootParameters, uint32_t numSamplers);
	void Create();
	void Bind(ID3D12GraphicsCommandList* cmdList);

	// アクセッサ
	ID3D12RootSignature* Get() const { return mRootSignature.Get(); }
	// ルートパラメータ
	RootParameter& RootParameters(uint32_t index)
	{
		MY_ASSERT(index >= 0 && index < mNumRootParameters);
		return mRootParameters[index];
	}
	// サンプラー
	D3D12_STATIC_SAMPLER_DESC& Samplers(uint32_t index)
	{
		MY_ASSERT(index >= 0 && index < mNumSamplers);
		return mSamplers[index];
	}

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	// ルートパラメータ
	uint32_t mNumRootParameters;
	std::unique_ptr<RootParameter[]> mRootParameters;
	// サンプラー
	uint32_t mNumSamplers;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
};
