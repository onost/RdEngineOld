#include "Mesh.h"
#include "Animation.h"
#include "Skeleton.h"
#include "Helper/MyAssert.h"
#include "Material.h"

Mesh::~Mesh()
{
	if (mSkeleton)
	{
		delete mSkeleton;
	}
}

void Mesh::Create()
{
	if (mVertices.size() > 0)
	{
		// 頂点バッファ
		mVBuff = std::make_unique<VertexBuffer>();
		mVBuff->Create(
			sizeof(Vertex) * static_cast<uint32_t>(mVertices.size()), sizeof(Vertex),
			static_cast<void*>(mVertices.data()));
	}
	if (mIndices.size() > 0)
	{
		// インデックスバッファ
		mIBuff = std::make_unique<IndexBuffer>();
		mIBuff->Create(
			sizeof(uint32_t) * static_cast<uint32_t>(mIndices.size()),
			static_cast<void*>(mIndices.data()));
	}

	// Skin Cluster
	if (mSkeleton)
	{
		mSkinCluster = std::make_unique<SkinCluster>();
		mSkinCluster->Create(this, mSkeleton);
	}
}

void Mesh::Update(Animation* animation, float time)
{
	if (mSkeleton && animation)
	{
		mCurrPoses = animation->UpdatePoseAtTime(mSkeleton, time);
		mSkinCluster->Update(mCurrPoses);
	}
}

// 描画
void Mesh::Draw(ID3D12GraphicsCommandList* cmdList,
	uint32_t matRootParamIdx, uint32_t texRootParamIdx)
{
	DrawInstancing(cmdList, 1, matRootParamIdx, texRootParamIdx);
}

// インスタンシング
void Mesh::DrawInstancing(ID3D12GraphicsCommandList* cmdList,
	uint32_t instanceCount, uint32_t matRootParamIdx, uint32_t texRootParamIdx)
{
	MY_ASSERT(cmdList);

	if (mMaterial)
	{
		// マテリアル
		mMaterial->Bind(cmdList, matRootParamIdx, texRootParamIdx);
		if (mVBuff)
		{
			if (mSkeleton)
			{
				D3D12_VERTEX_BUFFER_VIEW vbvs[2] =
				{
					mVBuff->GetView(),
					mSkinCluster->GetView()
				};
				cmdList->IASetVertexBuffers(0, 2, vbvs);
				mSkinCluster->Bind(cmdList, 5);
			}
			else
			{
				mVBuff->Bind(cmdList);
			}

			if (mIBuff)
			{
				// インデックスあり
				mIBuff->Bind(cmdList);
				cmdList->DrawIndexedInstanced(static_cast<uint32_t>(mIndices.size()), instanceCount, 0, 0, 0);
			}
			else
			{
				// インデックスなし
				cmdList->DrawInstanced(static_cast<uint32_t>(mVertices.size()), instanceCount, 0, 0);
			}
		}
	}
}

void Mesh::SetSkeleton(Skeleton* skeleton)
{
	mSkeleton = skeleton;
	if (mSkinCluster)
	{
		mSkinCluster->Create(this, mSkeleton);
	}
	else
	{
		mSkinCluster = std::make_unique<SkinCluster>();
		mSkinCluster->Create(this, mSkeleton);
	}
}
