#pragma once
#include "RootSignature.h"
#include "Shader.h"
#include <memory>

class PipelineState
{
public:
	PipelineState();
	void Create();
	void Bind(ID3D12GraphicsCommandList* cmdList);

	// ルートシグネチャ
	void SetRootSignature(RootSignature* rootSignature)
	{
		mDesc.pRootSignature = rootSignature->Get();
	}

	// 頂点シェーダ
	void SetVertexShader(Shader* vertexShader)
	{
		auto blob = vertexShader->GetBlob();
		mDesc.VS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.VS.BytecodeLength = blob->GetBufferSize();
	}

	// ジオメトリシェーダ
	void SetGeometryShader(Shader* geometryShader)
	{
		auto blob = geometryShader->GetBlob();
		mDesc.GS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.GS.BytecodeLength = blob->GetBufferSize();
	}

	// ピクセルシェーダ
	void SetPixelShader(Shader* pixelShader)
	{
		auto blob = pixelShader->GetBlob();
		mDesc.PS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.PS.BytecodeLength = blob->GetBufferSize();
	}

	// ブレンド
	void SetBlendState(D3D12_BLEND_DESC blend)
	{
		mDesc.BlendState = blend;
	}

	// ラスタライザ
	void SetRasterizerState(D3D12_RASTERIZER_DESC rasterizer)
	{
		mDesc.RasterizerState = rasterizer;
	}

	// 深度ステンシル
	void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC depthStencil)
	{
		mDesc.DepthStencilState = depthStencil;
	}

	// 頂点レイアウト
	void SetInputLayout(uint32_t numElements, D3D12_INPUT_ELEMENT_DESC inputElements[])
	{
		if (numElements > 0)
		{
			auto size = sizeof(D3D12_INPUT_ELEMENT_DESC) * numElements;
			D3D12_INPUT_ELEMENT_DESC* tmp = (D3D12_INPUT_ELEMENT_DESC*)(malloc(size));
			memcpy(tmp, inputElements, size);
			mDesc.InputLayout.pInputElementDescs = tmp;
			mInputElements.reset(tmp);
		}
		mDesc.InputLayout.NumElements = numElements;
	}

	// プリミティブトポロジー
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology)
	{
		mDesc.PrimitiveTopologyType = primitiveTopology;
	}

	ID3D12PipelineState* Get() const { return mPipelineState.Get(); }

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
	// 頂点レイアウト
	std::shared_ptr<D3D12_INPUT_ELEMENT_DESC> mInputElements;
};
