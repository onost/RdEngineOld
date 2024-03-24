#pragma once
#include "Helper/MyAssert.h"
#include "RootParameter.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

// ルートシグネチャ
class RootSignature
{
public:
	void Initialize(uint32_t numRootParams, uint32_t numSamplers);
	void Create();
	void Bind(ID3D12GraphicsCommandList* cmdList);

	// ルートパラメータへアクセス
	RootParameter& RootParams(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < mNumRootParams);
		return mRootParams[i];
	}
	// サンプラーへアクセス
	D3D12_STATIC_SAMPLER_DESC& Samplers(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < mNumSamplers);
		return mSamplers[i];
	}

	ID3D12RootSignature* Get() const { return mRootSignature.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	// ルートパラメータ
	uint32_t mNumRootParams;
	std::unique_ptr<RootParameter[]> mRootParams;
	// サンプラー
	uint32_t mNumSamplers;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
};
