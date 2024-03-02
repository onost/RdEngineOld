#pragma once
#include "Bone.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

class SkeletonOld;

class AnimationOld
{
public:
	// アニメーションを取得
	std::vector<Matrix4> GetPoseAtTime(SkeletonOld* skeleton, float time);

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



template <class T>
struct Keyframe
{
	float mTime;
	T mValue;
};

struct NodeAnimation
{
	std::vector<Keyframe<Vector3>> mScale;
	std::vector<Keyframe<Quaternion>> mRotate;
	std::vector<Keyframe<Vector3>> mTranslate;
};

// 値を計算
Vector3 CalcValue(
	const std::vector<Keyframe<Vector3>>& key, float time);
Quaternion CalcValue(
	const std::vector<Keyframe<Quaternion>>& key, float time);

/*
template <class T>
struct AnimationCurve
{
	std::vector<Keyframe<T>> mKeyframes;
};

struct NodeAnimation
{
	AnimationCurve<Vector3> mScale;
	AnimationCurve<Quaternion> mRotate;
	AnimationCurve<Vector3> mTranslate;
};
*/

struct Animation
{
	std::string mName;
	float mDuration;
	std::map<std::string, NodeAnimation> mNodeAnimations;
};
