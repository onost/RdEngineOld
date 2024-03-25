#pragma once
#include "Helper/MyAssert.h"
#include "RootParameter.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class RootSignature
{
public:
	RootSignature(uint32_t numRootParameters, uint32_t numSamplers);

	void Create();

	void Bind(ID3D12GraphicsCommandList* cmdList);

	RootParameter& RootParameters(uint32_t index)
	{
		MY_ASSERT(index >= 0 && index < mNumRootParameters);
		return mRootParameters[index];
	}
	D3D12_STATIC_SAMPLER_DESC& Samplers(uint32_t index)
	{
		MY_ASSERT(index >= 0 && index < mNumSamplers);
		return mSamplers[index];
	}

	ID3D12RootSignature* Get() const { return mRootSignature.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	uint32_t mNumRootParameters;
	uint32_t mNumSamplers;
	std::unique_ptr<RootParameter[]> mRootParameters;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
};
