#pragma once
#include "Bone.h"
#include <string>
#include <vector>

class Skeleton
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
