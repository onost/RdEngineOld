#include "ModelCommon.h"
#include "Core/GraphicsCommon.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Helper/MyAssert.h"

const std::string ModelCommon::kModelPath = "Assets/Model/";

ID3D12GraphicsCommandList* ModelCommon::mCmdList = nullptr;
Renderer* ModelCommon::mRenderer = nullptr;
RootSignature ModelCommon::mRootSignature;
PipelineState ModelCommon::mPsos[3];
PipelineState ModelCommon::mSkinnedPsos[3];
std::unique_ptr<ConstantBuffer> ModelCommon::mCBuff;

void ModelCommon::Initialize(Renderer* renderer)
{
	MyAssert(renderer);
	mRenderer = renderer;

	// ルートシグネチャ
	mRootSignature.Initialize(6, 1);
	mRootSignature.RootParams(0).InitConstant(0);
	mRootSignature.RootParams(1).InitConstant(1);
	mRootSignature.RootParams(2).InitConstant(2);
	mRootSignature.RootParams(3).InitDescTable(1);
	mRootSignature.RootParams(3).SetDescRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature.RootParams(4).InitConstant(3);
	mRootSignature.RootParams(5).InitDescTable(1);
	mRootSignature.RootParams(5).SetDescRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	mRootSignature.Samplers(0) = GraphicsCommon::gSamplerLinearWrap;
	mRootSignature.Create();

	// シェーダ
	Shader* defaultVs = renderer->GetVs("Assets/Shader/Model/DefaultVs.hlsl");
	Shader* skinnedVs = renderer->GetVs("Assets/Shader/Model/SkinnedVs.hlsl");
	Shader* defaultPs = renderer->GetPs("Assets/Shader/Model/DefaultPs.hlsl");
	Shader* unlightPs = renderer->GetPs("Assets/Shader/Model/UnlightPs.hlsl");
	// パイプラインステート
	mPsos[uint32_t(Type::Default)].SetRootSignature(mRootSignature.Get());
	mPsos[uint32_t(Type::Default)].SetVertexShader(defaultVs->Get());
	mPsos[uint32_t(Type::Default)].SetPixelShader(defaultPs->Get());
	mPsos[uint32_t(Type::Default)].SetBlendDesc(GraphicsCommon::gBlendNormal);
	mPsos[uint32_t(Type::Default)].SetRasterizerDesc(GraphicsCommon::gRasterizerDefault);
	mPsos[uint32_t(Type::Default)].SetDepthStencilDesc(GraphicsCommon::gDepthEnable);
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
	mPsos[uint32_t(Type::Default)].SetInputLayout(inputLayouts, _countof(inputLayouts));

	mPsos[uint32_t(Type::Unlight)] = mPsos[uint32_t(Type::Default)];
	mPsos[uint32_t(Type::Unlight)].SetPixelShader(unlightPs->Get());
	mPsos[uint32_t(Type::Wireframe)] = mPsos[uint32_t(Type::Default)];
	mPsos[uint32_t(Type::Wireframe)].SetRasterizerDesc(GraphicsCommon::gRasterizerFillModeWireframe);
	for (auto& p : mPsos)
	{
		p.Create();
	}

	// スキンアニメーション用
	mSkinnedPsos[uint32_t(Type::Default)].SetRootSignature(mRootSignature.Get());
	mSkinnedPsos[uint32_t(Type::Default)].SetVertexShader(skinnedVs->Get());
	mSkinnedPsos[uint32_t(Type::Default)].SetPixelShader(defaultPs->Get());
	mSkinnedPsos[uint32_t(Type::Default)].SetBlendDesc(GraphicsCommon::gBlendNormal);
	mSkinnedPsos[uint32_t(Type::Default)].SetRasterizerDesc(GraphicsCommon::gRasterizerDefault);
	mSkinnedPsos[uint32_t(Type::Default)].SetDepthStencilDesc(GraphicsCommon::gDepthEnable);
	D3D12_INPUT_ELEMENT_DESC skinnedInputLayouts[5] = {};
	skinnedInputLayouts[0].SemanticName = "POSITION";
	skinnedInputLayouts[0].SemanticIndex = 0;
	skinnedInputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	skinnedInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[1].SemanticName = "NORMAL";
	skinnedInputLayouts[1].SemanticIndex = 0;
	skinnedInputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	skinnedInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[2].SemanticName = "TEXCOORD";
	skinnedInputLayouts[2].SemanticIndex = 0;
	skinnedInputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	skinnedInputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[3].SemanticName = "WEIGHTS";
	skinnedInputLayouts[3].SemanticIndex = 0;
	skinnedInputLayouts[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	skinnedInputLayouts[3].InputSlot = 1;//
	skinnedInputLayouts[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[4].SemanticName = "BONEINDICES";
	skinnedInputLayouts[4].SemanticIndex = 0;
	skinnedInputLayouts[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	skinnedInputLayouts[4].InputSlot = 1;//
	skinnedInputLayouts[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mSkinnedPsos[uint32_t(Type::Default)].SetInputLayout(skinnedInputLayouts, _countof(skinnedInputLayouts));

	mSkinnedPsos[uint32_t(Type::Unlight)] = mSkinnedPsos[uint32_t(Type::Default)];
	mSkinnedPsos[uint32_t(Type::Unlight)].SetPixelShader(unlightPs->Get());
	mSkinnedPsos[uint32_t(Type::Wireframe)] = mSkinnedPsos[uint32_t(Type::Default)];
	mSkinnedPsos[uint32_t(Type::Wireframe)].SetRasterizerDesc(GraphicsCommon::gRasterizerFillModeWireframe);
	for (auto& p : mSkinnedPsos)
	{
		p.Create();
	}

	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Constant));
}

// レンダリング前
void ModelCommon::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	MyAssert(cmdList);
	mCmdList = cmdList;
	mRootSignature.Bind(mCmdList);
	mPsos[uint32_t(Type::Default)].Bind(mCmdList);// とりま
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// カメラ
	Camera* camera = mRenderer->GetCurrCamera();
	if (camera)
	{
		Constant data;
		data.mView = camera->GetView();
		data.mProj = camera->GetProj();
		data.mCameraPos = camera->GetPosition();
		mCBuff->Copy(data);
	}
	mCBuff->Bind(cmdList, 1);
	// ライト
	mRenderer->GetLightManager()->Bind(cmdList, 4);
}

// レンダリング後
void ModelCommon::PostRendering()
{
	mCmdList = nullptr;
}

void ModelCommon::SetPso(Type type)
{
	MyAssert(mCmdList);
	mPsos[uint32_t(type)].Bind(mCmdList);
}

void ModelCommon::SetSkinnedPso(Type type)
{
	MyAssert(mCmdList);
	mSkinnedPsos[uint32_t(type)].Bind(mCmdList);
}
