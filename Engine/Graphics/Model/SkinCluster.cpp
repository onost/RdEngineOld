#include "SkinCluster.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Helper/MyAssert.h"

void SkinCluster::Create(Mesh* mesh, Skeleton* skeleton)
{
	// Matrix Palette
	mPalette = new StructuredBuffer();
	mPalette->Create(sizeof(Well), uint32_t(skeleton->mJoints.size()));

	// Vertex Influence
	mInfluence = new VertexBuffer();
	mInfluence->Create(sizeof(VertexInfluence) * uint32_t(mesh->mVertices.size()), sizeof(VertexInfluence));
	VertexInfluence* influenceData = static_cast<VertexInfluence*>(mInfluence->GetData());

	mInvBindPose.resize(skeleton->mJoints.size());
	for (const auto& jointWeight : mesh->mSkinClusterData)
	{
		auto it = skeleton->mJointMap.find(jointWeight.first);
		if (it == skeleton->mJointMap.end())
		{
			continue;
		}

		mInvBindPose[(*it).second] = jointWeight.second.mInvBindPose;
		for (const auto& vertexWeight : jointWeight.second.mVertexWeights)
		{
			VertexInfluence& currInfluence = influenceData[vertexWeight.mVertexIndex];
			for (uint32_t i = 0; i < kMaxInfluence; ++i)
			{
				if (currInfluence.mWeights[i] == 0.0f)
				{
					currInfluence.mWeights[i] = vertexWeight.mWeight;
					currInfluence.mJointIndices[i] = (*it).second;
					break;
				}
			}
		}
	}
}

void SkinCluster::Update(Skeleton* skeleton)
{
	for (size_t i = 0; i < skeleton->mJoints.size(); ++i)
	{
		MyAssert(i < mInvBindPose.size());
		Well* palette = static_cast<Well*>(mPalette->GetData());
		palette[i].mSkeletonSpace = mInvBindPose[i] * skeleton->mJoints[i].mSkeletonSpaceMat;
	}
}
