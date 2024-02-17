#pragma once
#include "Core/Shader.h"
#include "RootSignature.h"
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <memory>

// パイプラインステート
class PipelineState
{
public:
	PipelineState();
	void Create();
	void Bind(ID3D12GraphicsCommandList* cmdList);

	// ルートシグネチャ
	void SetRootSignature(ID3D12RootSignature* rootSignature)
	{
		mDesc.pRootSignature = rootSignature;
	}
	// 頂点シェーダ
	void SetVertexShader(IDxcBlob* blob)
	{
		mDesc.VS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.VS.BytecodeLength = blob->GetBufferSize();
	}
	// ジオメトリシェーダ
	void SetGeometryShader(IDxcBlob* blob)
	{
		mDesc.GS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.GS.BytecodeLength = blob->GetBufferSize();
	}
	// ピクセルシェーダ
	void SetPixelShader(IDxcBlob* blob)
	{
		mDesc.PS.pShaderBytecode = blob->GetBufferPointer();
		mDesc.PS.BytecodeLength = blob->GetBufferSize();
	}
	// ブレンド
	void SetBlendDesc(D3D12_BLEND_DESC blendDesc)
	{
		mDesc.BlendState = blendDesc;
	}
	// ラスタライザ
	void SetRasterizerDesc(D3D12_RASTERIZER_DESC rasterizerDesc)
	{
		mDesc.RasterizerState = rasterizerDesc;
	}
	// 深度ステンシル
	void SetDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC depthStencilDesc)
	{
		mDesc.DepthStencilState = depthStencilDesc;
	}
	// 頂点レイアウト
	void SetInputLayout(
		D3D12_INPUT_ELEMENT_DESC inputLayouts[], uint32_t numElements)
	{
		mDesc.InputLayout.NumElements = numElements;
		if (numElements > 0)
		{
			auto size = sizeof(D3D12_INPUT_ELEMENT_DESC) * numElements;
			D3D12_INPUT_ELEMENT_DESC* tmp = (D3D12_INPUT_ELEMENT_DESC*)(malloc(size));
			memcpy(tmp, inputLayouts, size);
			mInputLayouts.reset(tmp);
			mDesc.InputLayout.pInputElementDescs = mInputLayouts.get();
		}
	}
	// プリミティブトポロジー
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE primTopologyType)
	{
		mDesc.PrimitiveTopologyType = primTopologyType;
	}

	ID3D12PipelineState* Get() const { return mPipelineState.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
	// 頂点レイアウト
	std::shared_ptr<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
};
