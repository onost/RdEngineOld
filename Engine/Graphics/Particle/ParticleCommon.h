#pragma once
#include "Color.h"
#include "Core/ConstantBuffer.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include <string>

class Camera;
class Renderer;

class ParticleCommon
{
	friend class ParticleManager;
	friend class ParticleRenderer;
	friend class MeshParticleRenderer;
public:
	// 頂点
	struct Vertex
	{
		Vector3 mPos;
		Vector3 mNormal;
		Vector2 mUv;
	};
	// 定数
	struct Constant
	{
		Matrix4 mView;
		Matrix4 mProj;
		Vector3 mCameraPos;
	};
	// ブレンド
	enum class Blend
	{
		None,
		Normal,
		Add,
		Subtract,
		Multiply,
		Screen
	};

	static void Initialize(Renderer* renderer);
	//void Terminate();

	// レンダリング前
	static void PreRendering(ID3D12GraphicsCommandList* cmdList);
	// レンダリング後
	static void PostRendering();

	static void SetBlend(Blend blend);

private:
	static ID3D12GraphicsCommandList* mCmdList;
	static Renderer* mRenderer;
	static Camera* mCamera;
	static std::unique_ptr<RootSignature> mRootSignature;
	static PipelineState mPsos[6];
	static PipelineState mModelPsos[3];
	static std::unique_ptr<ConstantBuffer> mCBuff;
};
