#include "Animation.h"
#include "Skeleton.h"
#include "Helper/MyAssert.h"

std::vector<Matrix4> AnimationOld::GetPoseAtTime(SkeletonOld* skeleton, float time)
{
	MyAssert(skeleton);
	uint32_t currFrame = static_cast<uint32_t>(time / mFrameDuration);
	uint32_t nextFrame = currFrame + 1;
	float t = time / mFrameDuration - currFrame;

	auto& bones = skeleton->GetBones();
	std::vector<Matrix4> result(bones.size());
	// ルート
	BoneTransform interpolate =
		BoneTransform::Interpolate(
			mTracks[currFrame][bones[0].mName],
			mTracks[nextFrame][bones[0].mName],
			t);
	result[0] = interpolate.GetMatrix();
	// ルート以外
	for (uint32_t i = 1; i < bones.size(); ++i)
	{
		interpolate =
			BoneTransform::Interpolate(
				mTracks[currFrame][bones[i].mName],
				mTracks[nextFrame][bones[i].mName],
				t);
		result[i] = interpolate.GetMatrix() * result[bones[i].mParent];
	}

	return result;
}

// Vector3
Vector3 CalcValue(const std::vector<Keyframe<Vector3>>& key, float time)
{
	MyAssert(!key.empty());
	if (key.size() == 1 || time <= key[0].mTime)
	{
		return key[0].mValue;
	}
	for (uint32_t i = 0; i < key.size() - 1; ++i)
	{
		uint32_t next = i + 1;
		if (key[i].mTime <= time && time <= key[next].mTime)
		{
			float t = (time - key[i].mTime) / (key[next].mTime - key[i].mTime);
			return MyMath::Lerp(key[i].mValue, key[next].mValue, t);
		}
	}
	return (*key.rbegin()).mValue;
}

// Quaternion
Quaternion CalcValue(const std::vector<Keyframe<Quaternion>>& key, float time)
{
	MyAssert(!key.empty());
	if (key.size() == 1 || time <= key[0].mTime)
	{
		return key[0].mValue;
	}
	for (uint32_t i = 0; i < key.size() - 1; ++i)
	{
		uint32_t next = i + 1;
		if (key[i].mTime <= time && time <= key[next].mTime)
		{
			float t = (time - key[i].mTime) / (key[next].mTime - key[i].mTime);
			return Slerp(key[i].mValue, key[next].mValue, t);
		}
	}
	return (*key.rbegin()).mValue;
}
