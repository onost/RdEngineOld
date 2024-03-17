#include "SkinCluster.h"
#include "Mesh.h"
#include "Skeleton.h"

void SkinCluster::Create(Mesh* mesh, Skeleton* skeleton)
{
	// 頂点
	mInfluence = std::make_unique<VertexBuffer>();
	mInfluence->Create(
		sizeof(VertexInfluence) * uint32_t(mesh->GetVertices().size()),
		sizeof(VertexInfluence));

	auto joints = skeleton->GetJoints();
	mInvBindPoses.resize(joints.size());
	// Matrix Palette
	mPalette = std::make_unique<StructuredBuffer>();
	mPalette->Create(sizeof(Well), uint32_t(joints.size()));

	auto jointMap = skeleton->GetJointMap();
	VertexInfluence* influenceData = static_cast<VertexInfluence*>(mInfluence->GetData());
	for (const auto& data : skeleton->GetForSkinCluster())
	{
		auto it = jointMap.find(data.first);
		if (it == jointMap.end())
		{
			continue;
		}
		// 逆バインドポーズ行列
		mInvBindPoses[(*it).second] = data.second.mInvBindPose;
		// ウェイト
		for (const auto& w : data.second.mWeightData)
		{
			VertexInfluence& curr = influenceData[w.mVertexIndex];
			for (uint32_t i = 0; i < kMaxInfluence; ++i)
			{
				if (curr.mWeights[i] == 0.0f)
				{
					curr.mWeights[i] = w.mWeight;
					curr.mJointIndices[i] = (*it).second;
					break;
				}
			}
		}
	}
}

void SkinCluster::Update(const std::vector<Matrix4>& poses)
{
	// パレットを更新
	Well* palette = static_cast<Well*>(mPalette->GetData());
	for (size_t i = 0; i < poses.size(); ++i)
	{
		palette[i].mSkelSpaceMat = mInvBindPoses[i] * poses[i];
	}
}

void SkinCluster::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx)
{
	mPalette->Bind(cmdList, rootParamIdx);
}
