#pragma once
#include "Collision/Collision.h"
#include "Color.h"
#include "Core/ConstantBuffer.h"
#include "Core/PipelineState.h"
#include "Core/RootSignature.h"
#include "Core/VertexBuffer.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <vector>

class Renderer;

class Primitive
{
private:
	struct Vertex
	{
		Vector3 mPos;
		Color mColor;
	};

public:
	void Initialize(Renderer* renderer);
	//void Terminate();
	// レンダリング前
	void PreRendering(ID3D12GraphicsCommandList* cmdList);
	// レンダリング後
	void PostRendering();

	void DrawLine2(const Vector2& p1, const Vector2& p2, const Color& col);
	void DrawLine3(const Vector3& p1, const Vector3& p2, const Color& col);
	void DrawTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Color& col, bool isFill = true);
	void DrawBox2(const Vector2& pos, const Vector2& size, const Color& col, bool isFill = true);
	void DrawBox2(const Vector2& pos, const Vector2& size, float r, const Color& col, bool isFill = true);
	void DrawBox3(const Vector3& pos, const Vector3& size, const Color& col);
	//void DrawBox3(const Vector3& min, const Vector3& max, const Color& col);
	void DrawBox3(const AABB& aabb, const Color& col);// Collider
	void DrawBox3(const OBB& obb, const Color& col);// Collider
	void DrawCircle(const Vector2& pos, float rad, const Color& col, bool isFill = true);
	void DrawSphere(const Vector3& pos, float rad, const Color& col);

	// Grid
	void DrawGrid(uint32_t gridNum = 201);

private:
	// Line
	void LineList2(const std::vector<Vertex>& v);
	void LineList3(const std::vector<Vertex>& v);
	// Triangle
	void TriList2(const std::vector<Vertex>& v);

private:
	static const uint32_t kMaxVertex = 16384;
	ID3D12GraphicsCommandList* mCmdList;
	Renderer* mRenderer;
	std::unique_ptr<RootSignature> mRootSignature;
	PipelineState mLinePso2;
	PipelineState mLinePso3;
	PipelineState mPrimPso2;
	PipelineState mPrimPso3;
	PipelineState mGridPso;
	std::unique_ptr<VertexBuffer> mVBuff;
	std::unique_ptr<ConstantBuffer> mCBuff2;
	std::unique_ptr<ConstantBuffer> mCBuff3;
	uint32_t mIndex;
};
