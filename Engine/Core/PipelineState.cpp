#include "PipelineState.h"
#include "GraphicsEngine.h"

PipelineState::PipelineState()
	: mDesc()
	, mPipelineState(nullptr)
	, mInputElements(nullptr)
{
	// デフォルトの設定
	mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mDesc.NumRenderTargets = 1;
	mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	mDesc.SampleDesc.Count = 1;
}

void PipelineState::Create()
{
	// グラフィックスパイプラインを作成
	[[maybe_unused]] HRESULT hr = gGraphicsEngine->GetDevice()->CreateGraphicsPipelineState(
		&mDesc, IID_PPV_ARGS(&mPipelineState));
	MY_ASSERT(SUCCEEDED(hr));
}

void PipelineState::Bind(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	cmdList->SetPipelineState(mPipelineState.Get());
}
