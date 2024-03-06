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

// アニメーション
class Animation
{
	friend class ModelLoader;
public:
	std::vector<Matrix4> UpdatePoseAtTime(Skeleton* skeleton, float time);

	const std::string& GetName() const { return mName; }
	const std::map<std::string, NodeAnimation>& GetNodeAnimations() const { return mNodeAnimations; }
	float GetDuration() const { return mDuration; }

private:
	std::string mName;
	// string: アニメーション名
	// NodeAnimation: アニメーション
	std::map<std::string, NodeAnimation> mNodeAnimations;
	float mDuration;
};
