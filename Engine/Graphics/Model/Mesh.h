#pragma once
#include "Core/IndexBuffer.h"
#include "Core/VertexBuffer.h"
#include "Matrix4.h"
#include "SkinCluster.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <vector>

class Animation;
class Material;

class Mesh
{
	friend class ObjLoader;
	friend class ModelLoader;
public:
	// 頂点
	struct Vertex
	{
		Vector3 mPos;
		Vector3 mNormal;
		Vector2 mUv;
	};

public:
	~Mesh();

	// 頂点、インデックスを追加
	void AddVertex(const Vertex vertex) { mVertices.emplace_back(vertex); }
	void AddIndex(const uint32_t index) { mIndices.emplace_back(index); }

	void Create();
	// スケルトンなどの更新
	void Update(Animation* animation, float time);
	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList,
		uint32_t matRootParamIdx, uint32_t texRootParamIdx);
	// インスタンシング
	void DrawInstancing(ID3D12GraphicsCommandList* cmdList,
		uint32_t instanceCount, uint32_t matRootParamIdx, uint32_t texRootParamIdx);

	// Getter
	std::vector<Vertex>& GetVertices() { return mVertices; }
	std::vector<uint32_t>& GetIndices() { return mIndices; }
	Material* GetMaterial() const { return mMaterial; }
	bool GetIsSkinned() const { return mSkeleton != nullptr; }
	const Matrix4 GetLocal() const { return mLocal; }
	Skeleton* GetSkeleton() const { return mSkeleton; }
	const std::vector<Matrix4>& GetCurrPoses() const { return mCurrPoses; }
	// Setter
	void SetMaterial(Material* material) { mMaterial = material; }
	void SetSkeleton(Skeleton* skeleton);// cpp

private:
	std::unique_ptr<VertexBuffer> mVBuff;
	std::unique_ptr<IndexBuffer> mIBuff;
	// 頂点
	std::vector<Vertex> mVertices;
	std::unique_ptr<SkinCluster> mSkinCluster;
	// インデックス
	std::vector<uint32_t> mIndices;

	// マテリアル
	Material* mMaterial;
	// スケルトン
	Skeleton* mSkeleton;
	std::vector<Matrix4> mCurrPoses;

	// トランスフォーム
	Matrix4 mLocal;
};
