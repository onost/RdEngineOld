#pragma once
#include "Matrix4.h"
#include "Core/StructuredBuffer.h"
#include "Core/VertexBuffer.h"
#include <array>
#include <vector>

class Mesh;
struct Skeleton;

const uint32_t kMaxInfluence = 4;
struct VertexInfluence
{
	std::array<float, kMaxInfluence> mWeights;
	std::array<int32_t, kMaxInfluence> mJointIndices;
};

struct Well
{
	Matrix4 mSkeletonSpace;
	// 法線用（非均一スケール）
};

struct SkinCluster
{
	std::vector<Matrix4> mInvBindPose;
	VertexBuffer* mInfluence;
	StructuredBuffer* mPalette;

	void Create(Mesh* mesh, Skeleton* skeleton);
	void Update(Skeleton* skeleton);
};
