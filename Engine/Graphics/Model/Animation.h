#pragma once
#include "Bone.h"
#include <string>
#include <unordered_map>
#include <vector>

class Skeleton;

class Animation
{
public:
	// アニメーションを取得
	std::vector<Matrix4> GetPoseAtTime(Skeleton* skeleton, float time);

	const std::string& GetName() const { return mName; }
	uint32_t GetBoneCount() const { return mBoneCount; }
	float GetDuration() const { return mDuration; }

private:
	std::string mName;
	uint32_t mBoneCount;
	float mDuration;
	float mFrameDuration = 60.0f;
	// vector: Frame
	// unordered_map: Bone Name, Transform
	std::vector<std::unordered_map<std::string, BoneTransform>> mTracks;
};
