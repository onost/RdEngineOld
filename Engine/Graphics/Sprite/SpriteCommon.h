#pragma once
#include "Color.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include <d3d12.h>
#include <memory>

class Renderer;

class SpriteCommon
{
	friend class Sprite;
public:
	// 頂点
	struct Vertex
	{
		Vector3 mPos;
		Vector2 mUv;
	};
	// 定数
	struct Constant
	{
		Matrix4 mWvp;
		Color mColor;
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
	static std::unique_ptr<RootSignature> mRootSignature;
	static PipelineState mPipelineStates[6];
	static Matrix4 mProjMat;
};
