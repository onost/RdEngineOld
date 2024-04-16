#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include <map>
#include <string>
#include <vector>

class Skeleton;

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

// ==================================================
// ヘルパー関数
// ==================================================
Vector3 UpdateKeyframeAtTime(
	const std::vector<Keyframe<Vector3>>& key, float time);
Quaternion UpdateKeyframeAtTime(
	const std::vector<Keyframe<Quaternion>>& key, float time);
Vector3 UpdateKeyframeAtTime(
	const std::vector<Keyframe<Vector3>>& curr,
	const std::vector<Keyframe<Vector3>>& next, float time, float nextTime);
Quaternion UpdateKeyframeAtTime(
	const std::vector<Keyframe<Quaternion>>& key,
	const std::vector<Keyframe<Quaternion>>& next, float time, float nextTime);

// アニメーション
class Animation
{
	friend class ModelLoader;
public:
	std::vector<Matrix4> UpdatePoseAtTime(Skeleton* skeleton, float time);
	std::vector<Matrix4> UpdatePoseAtTime(Skeleton* skeleton, Animation* next, float time, float nextTime, float t);

	const std::string& GetName() const { return mName; }
	const std::map<std::string, NodeAnimation>& GetNodeAnimations() const { return mNodeAnimations; }
	float GetDuration() const { return mDuration; }
	bool GetIsLoop() const { return mIsLoop; }
	void SetIsLoop(bool isLoop) { mIsLoop = isLoop; }

private:
	std::string mName;
	// string: アニメーション名
	// NodeAnimation: アニメーション
	std::map<std::string, NodeAnimation> mNodeAnimations;
	float mDuration;
	bool mIsLoop = false;
};
