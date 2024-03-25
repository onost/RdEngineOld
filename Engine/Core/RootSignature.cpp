#include "RootSignature.h"
#include "GraphicsEngine.h"

RootSignature::RootSignature(uint32_t numRootParameters, uint32_t numSamplers)
	: mRootSignature(nullptr)
	, mNumRootParameters(numRootParameters)
	, mNumSamplers(numSamplers)
	, mRootParameters(nullptr)
	, mSamplers(nullptr)
{
	if (mNumRootParameters > 0)
	{
		mRootParameters.reset(new RootParameter[mNumRootParameters]);
	}
	if (mNumSamplers > 0)
	{
		mSamplers.reset(new D3D12_STATIC_SAMPLER_DESC[mNumSamplers]);
	}
}



void RootSignature::Create()
{
	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = mNumRootParameters;
	desc.pParameters = &mRootParameters.get()->mRootParameter;
	desc.NumStaticSamplers = mNumSamplers;
	desc.pStaticSamplers = mSamplers.get();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// シリアライズ
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rsBlob, &errorBlob);
	MY_ASSERT(SUCCEEDED(hr));

	// ルートシグネチャを作成
	hr = gGraphicsEngine->GetDevice()->CreateRootSignature(
		0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	MY_ASSERT(SUCCEEDED(hr));
}

void RootSignature::Bind(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(mRootSignature.Get());
}
