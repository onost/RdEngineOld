#include "ParticleCommon.h"
#include "Core/GraphicsCommon.h"
#include "Graphics/Camera.h"
#include "Graphics/Model/ModelCommon.h"
#include "Graphics/Renderer.h"

ID3D12GraphicsCommandList* ParticleCommon::mCmdList = nullptr;
Renderer* ParticleCommon::mRenderer = nullptr;
Camera* ParticleCommon::mCamera = nullptr;
std::unique_ptr<RootSignature> ParticleCommon::mRootSignature;
PipelineState ParticleCommon::mPsos[6];
PipelineState ParticleCommon::mModelPsos[3];
std::unique_ptr<ConstantBuffer> ParticleCommon::mCBuff;

void ParticleCommon::Initialize(Renderer* renderer)
{
	MY_ASSERT(renderer);
	mRenderer = renderer;

	// ルートシグネチャ
	//mRootSignature.Initialize(5, 1);
	mRootSignature = std::make_unique<RootSignature>(5, 1);
	mRootSignature->RootParameters(0).InitDescriptorTable(1);
	mRootSignature->RootParameters(0).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->RootParameters(1).InitConstant(1);
	mRootSignature->RootParameters(2).InitConstant(2);
	mRootSignature->RootParameters(3).InitDescriptorTable(1);
	mRootSignature->RootParameters(3).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	mRootSignature->RootParameters(4).InitConstant(3);
	mRootSignature->Samplers(0) = GraphicsCommon::gSamplerLinearWrap;
	mRootSignature->Create();

	// シェーダ
	Shader* vs = renderer->GetVs("Assets/Shader/Particle/ParticleVs.hlsl");
	Shader* defaultPs = renderer->GetPs("Assets/Shader/Particle/ParticlePs.hlsl");
	Shader* unlightPs = renderer->GetPs("Assets/Shader/Particle/UnlightParticlePs.hlsl");
	// パイプラインステート
	mPsos[uint32_t(Blend::None)].SetRootSignature(mRootSignature.get());
	mPsos[uint32_t(Blend::None)].SetVertexShader(vs);
	mPsos[uint32_t(Blend::None)].SetPixelShader(unlightPs);
	mPsos[uint32_t(Blend::None)].SetBlendState(GraphicsCommon::gBlendNone);
	mPsos[uint32_t(Blend::None)].SetRasterizerState(GraphicsCommon::gRasterizerCullModeNone);
	mPsos[uint32_t(Blend::None)].SetDepthStencilState(GraphicsCommon::gDepthWriteMaskZero);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[3] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "NORMAL";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[2].SemanticName = "TEXCOORD";
	inputLayouts[2].SemanticIndex = 0;
	inputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPsos[uint32_t(Blend::None)].SetInputLayout(_countof(inputLayouts), inputLayouts);

	mPsos[uint32_t(Blend::Normal)] = mPsos[uint32_t(Blend::None)];
	mPsos[uint32_t(Blend::Normal)].SetBlendState(GraphicsCommon::gBlendNormal);
	mPsos[uint32_t(Blend::Add)] = mPsos[uint32_t(Blend::None)];
	mPsos[uint32_t(Blend::Add)].SetBlendState(GraphicsCommon::gBlendAddition);
	mPsos[uint32_t(Blend::Subtract)] = mPsos[uint32_t(Blend::None)];
	mPsos[uint32_t(Blend::Subtract)].SetBlendState(GraphicsCommon::gBlendSubtract);
	mPsos[uint32_t(Blend::Multiply)] = mPsos[uint32_t(Blend::None)];
	mPsos[uint32_t(Blend::Multiply)].SetBlendState(GraphicsCommon::gBlendMultiply);
	mPsos[uint32_t(Blend::Screen)] = mPsos[uint32_t(Blend::None)];
	mPsos[uint32_t(Blend::Screen)].SetBlendState(GraphicsCommon::gBlendScreen);
	for (auto& p : mPsos)
	{
		p.Create();
	}

	// メッシュパーティクル用
	mModelPsos[uint32_t(ModelCommon::Type::Default)] = mPsos[uint32_t(Blend::None)];
	mModelPsos[uint32_t(ModelCommon::Type::Default)].SetPixelShader(defaultPs);
	mModelPsos[uint32_t(ModelCommon::Type::Default)].SetBlendState(GraphicsCommon::gBlendNormal);
	mModelPsos[uint32_t(ModelCommon::Type::Default)].SetDepthStencilState(GraphicsCommon::gDepthEnable);
	mModelPsos[uint32_t(ModelCommon::Type::Unlight)] = mModelPsos[uint32_t(ModelCommon::Type::Default)];
	mModelPsos[uint32_t(ModelCommon::Type::Unlight)].SetPixelShader(unlightPs);
	mModelPsos[uint32_t(ModelCommon::Type::Wireframe)] = mModelPsos[uint32_t(ModelCommon::Type::Default)];
	mModelPsos[uint32_t(ModelCommon::Type::Wireframe)].SetRasterizerState(GraphicsCommon::gRasterizerFillModeWireframe);
	for (auto& p : mModelPsos)
	{
		p.Create();
	}

	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Constant));
}

// レンダリング前
void ParticleCommon::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	MY_ASSERT(cmdList);
	mCmdList = cmdList;
	mRootSignature->Bind(mCmdList);
	mPsos[uint32_t(Blend::Normal)].Bind(mCmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// カメラ
	mCamera = mRenderer->GetCurrCamera();
	if (mCamera)
	{
		Constant data;
		data.mView = mCamera->GetView();
		data.mProj = mCamera->GetProj();
		data.mCameraPos = mCamera->GetPosition();
		mCBuff->Copy(data);
	}
	mCBuff->Bind(cmdList, 1);
	// ライト
	mRenderer->GetLightManager()->Bind(cmdList, 4);
}

// レンダリング後
void ParticleCommon::PostRendering()
{
	mCmdList = nullptr;
}

void ParticleCommon::SetBlend(Blend blend)
{
	MY_ASSERT(mCmdList);
	mPsos[uint32_t(blend)].Bind(mCmdList);
}
