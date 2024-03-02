#pragma once
#include "Bone.h"
#include "QuaternionTransform.h"
#include <string>
#include <vector>
#include <optional>
#include <map>
#include "Graphics/Model/Animation.h"

class SkeletonOld
{
public:
	// グローバルバインドポーズの逆行列を計算
	void ComputeInvBindPose();

	const std::string& GetName() const { return mName; }
	uint32_t GetBoneCount() const { return static_cast<uint32_t>(mBones.size()); }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetInvBindPoses() const { return mInvBindPoses; }

private:
	std::string mName;
	std::vector<Bone> mBones;
	// グローバルバインドポーズの逆行列
	std::vector<Matrix4> mInvBindPoses;
};



struct Joint
{
	std::string mName;
	QuaternionTransform mTransform;
	Matrix4 mLocal;
	Matrix4 mSkeletonSpaceMat;
	int32_t mIndex;
	std::optional<int32_t> mParent;
	std::vector<int32_t> mChildren;
};

struct Skeleton
{
	std::string mName;
	int32_t mRoot;
	std::map<std::string, int32_t> mJointMap;
	std::vector<Joint> mJoints;

	void Update();
	void ApplyAnimation(const Animation& animation, float time);
};
