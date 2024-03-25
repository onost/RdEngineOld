#pragma once
#include "Core/ConstantBuffer.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Matrix4.h"
#include "Vector3.h"
#include <string>

class Renderer;

class ModelCommon
{
	friend class Model;
	friend class MeshRenderer;//
	friend class SkinnedMeshRenderer;//
public:
	static const std::string kModelPath;
	// 定数
	struct Constant
	{
		Matrix4 mView;
		Matrix4 mProj;
		Vector3 mCameraPos;
	};
	// シェーダタイプ
	enum class Type
	{
		Default,
		Unlight,
		Wireframe,
		//Transparent	// 透過
	};

	static void Initialize(Renderer* renderer);
	//void Terminate();

	// レンダリング前
	static void PreRendering(ID3D12GraphicsCommandList* cmdList);
	// レンダリング後
	static void PostRendering();

	static void SetPso(Type type);
	static void SetSkinnedPso(Type type);

private:
	static ID3D12GraphicsCommandList* mCmdList;
	static Renderer* mRenderer;
	static std::unique_ptr<RootSignature> mRootSignature;
	static PipelineState mPsos[3];
	static PipelineState mSkinnedPsos[3];
	static std::unique_ptr<ConstantBuffer> mCBuff;
};
