#pragma once
#include "Core/IndexBuffer.h"
#include "Core/VertexBuffer.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <vector>
#include "Loader/ModelLoader.h"
#include "SkinCluster.h"

class Material;
struct SkinCluster;

class Mesh
{
	friend class ObjLoader;
	friend class ModelLoader;
	friend struct SkinCluster;
public:
	// 頂点
	struct Vertex
	{
		Vector3 mPos;
		Vector3 mNormal;
		Vector2 mUv;
		//uint32_t mBone[4];
		//float mWeight[4];
	};

public:
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
	bool GetIsSkinned() const { return mIsSkinned; }
	const Matrix4 GetLocal() const { return mLocal; }
	Skeleton* GetSkeleton() const { return mSkeleton; }
	// Setter
	void SetMaterial(Material* material) { mMaterial = material; }
	//void SetIsSkinned(bool isSkinned) { mIsSkinned = isSkinned; }

private:
	std::unique_ptr<VertexBuffer> mVBuff;
	std::unique_ptr<IndexBuffer> mIBuff;
	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
	Material* mMaterial;
	bool mIsSkinned;

	// トランスフォーム
	Matrix4 mLocal;

	// Skinning
	std::map<std::string, ModelLoader::JointWeightData> mSkinClusterData;
	Skeleton* mSkeleton;
	std::unique_ptr<SkinCluster> mSkinCluster;
};
