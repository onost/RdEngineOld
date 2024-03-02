#include "Mesh.h"
#include "Helper/MyAssert.h"
#include "Material.h"

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
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList,
	uint32_t matRootParamIdx, uint32_t texRootParamIdx)
{
	DrawInstancing(cmdList, 1, matRootParamIdx, texRootParamIdx);
}

// インスタンシング
void Mesh::DrawInstancing(ID3D12GraphicsCommandList* cmdList,
	uint32_t instanceCount, uint32_t matRootParamIdx, uint32_t texRootParamIdx)
{
	MyAssert(cmdList);
	if (mMaterial)
	{
		// マテリアル
		mMaterial->Bind(cmdList, matRootParamIdx, texRootParamIdx);
		if (mVBuff)
		{
			mVBuff->Bind(cmdList);
			if (mIBuff)
			{
				// インデックスあり
				mIBuff->Bind(cmdList);
				cmdList->DrawIndexedInstanced(
					static_cast<uint32_t>(mIndices.size()), instanceCount, 0, 0, 0);
			}
			else
			{
				// インデックスなし
				cmdList->DrawInstanced(
					static_cast<uint32_t>(mVertices.size()), instanceCount, 0, 0);
			}
		}
	}
}
