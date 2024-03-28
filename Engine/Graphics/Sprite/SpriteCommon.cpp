#include "SpriteCommon.h"
#include "Core/GraphicsCommon.h"
#include "Helper/MyAssert.h"
#include "Graphics/Renderer.h"
#include "Sprite.h"
#include "Window.h"

ID3D12GraphicsCommandList* SpriteCommon::mCmdList = nullptr;
std::unique_ptr<RootSignature> SpriteCommon::mRootSignature;
PipelineState SpriteCommon::mPipelineStates[6];
Matrix4 SpriteCommon::mProjMat = Matrix4::kIdentity;

void SpriteCommon::Initialize(Renderer* renderer)
{
	MY_ASSERT(renderer);

	mRootSignature = std::make_unique<RootSignature>(2, 1);
	// ルートシグネチャ
	//mRootSignature.Initialize(2, 1);
	mRootSignature->RootParameters(0).InitConstant(0);
	mRootSignature->RootParameters(1).InitDescriptorTable(1);
	mRootSignature->RootParameters(1).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->Samplers(0) = GraphicsCommon::gSamplerLinearClamp;
	mRootSignature->Create();

	// シェーダ
	Shader* vs = renderer->GetVs("Assets/Shader/Sprite/SpriteVs.hlsl");
	Shader* ps = renderer->GetPs("Assets/Shader/Sprite/SpritePs.hlsl");
	// パイプラインステート
	mPipelineStates[uint32_t(Blend::None)].SetRootSignature(mRootSignature.get());
	mPipelineStates[uint32_t(Blend::None)].SetVertexShader(vs);
	mPipelineStates[uint32_t(Blend::None)].SetPixelShader(ps);
	mPipelineStates[uint32_t(Blend::None)].SetBlendState(GraphicsCommon::gBlendNone);
	mPipelineStates[uint32_t(Blend::None)].SetRasterizerState(GraphicsCommon::gRasterizerCullModeNone);
	mPipelineStates[uint32_t(Blend::None)].SetDepthStencilState(GraphicsCommon::gDepthDisable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "TEXCOORD";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPipelineStates[uint32_t(Blend::None)].SetInputLayout(_countof(inputLayouts), inputLayouts);

	mPipelineStates[uint32_t(Blend::Normal)] = mPipelineStates[uint32_t(Blend::None)];
	mPipelineStates[uint32_t(Blend::Normal)].SetBlendState(GraphicsCommon::gBlendNormal);
	mPipelineStates[uint32_t(Blend::Add)] = mPipelineStates[uint32_t(Blend::None)];
	mPipelineStates[uint32_t(Blend::Add)].SetBlendState(GraphicsCommon::gBlendAddition);
	mPipelineStates[uint32_t(Blend::Subtract)] = mPipelineStates[uint32_t(Blend::None)];
	mPipelineStates[uint32_t(Blend::Subtract)].SetBlendState(GraphicsCommon::gBlendSubtract);
	mPipelineStates[uint32_t(Blend::Multiply)] = mPipelineStates[uint32_t(Blend::None)];
	mPipelineStates[uint32_t(Blend::Multiply)].SetBlendState(GraphicsCommon::gBlendMultiply);
	mPipelineStates[uint32_t(Blend::Screen)] = mPipelineStates[uint32_t(Blend::None)];
	mPipelineStates[uint32_t(Blend::Screen)].SetBlendState(GraphicsCommon::gBlendScreen);
	for (auto& p : mPipelineStates)
	{
		p.Create();
	}

	// プロジェクション行列
	mProjMat = Matrix4::CreateOrthographic(
		0.0f, 0.0f, float(Window::kWidth), float(Window::kHeight), 0.0f, 100.0f);
}

// レンダリング前
void SpriteCommon::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	mCmdList = cmdList;
	mRootSignature->Bind(mCmdList);
	mPipelineStates[uint32_t(Blend::Normal)].Bind(mCmdList);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

// レンダリング後
void SpriteCommon::PostRendering()
{
	mCmdList = nullptr;
}

// パイプラインステートをセット
void SpriteCommon::SetBlend(Blend blend)
{
	MY_ASSERT(mCmdList);
	mPipelineStates[uint32_t(blend)].Bind(mCmdList);
}
