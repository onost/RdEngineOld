#include "Animation.h"
#include "Skeleton.h"
#include "Helper/MyAssert.h"

// Vector3
Vector3 UpdateKeyframeAtTime(
	const std::vector<Keyframe<Vector3>>& key, float time)
{
	MY_ASSERT(!key.empty());
	if (key.size() == 1 || time <= key[0].mTime)
	{
		return key[0].mValue;
	}
	for (uint32_t curr = 0; curr < key.size() - 1; ++curr)
	{
		uint32_t next = curr + 1;
		if (key[curr].mTime <= time &&
			key[next].mTime >= time)
		{
			float t = (time - key[curr].mTime) / (key[next].mTime - key[curr].mTime);
			return MyMath::Lerp(key[curr].mValue, key[next].mValue, t);
		}
	}
	return (*key.rbegin()).mValue;
}

// Quaternion
Quaternion UpdateKeyframeAtTime(
	const std::vector<Keyframe<Quaternion>>& key, float time)
{
	MY_ASSERT(!key.empty());
	if (key.size() == 1 || time <= key[0].mTime)
	{
		return key[0].mValue;
	}
	for (uint32_t curr = 0; curr < key.size() - 1; ++curr)
	{
		uint32_t next = curr + 1;
		if (key[curr].mTime <= time &&
			key[next].mTime >= time)
		{
			float t = (time - key[curr].mTime) / (key[next].mTime - key[curr].mTime);
			return Slerp(key[curr].mValue, key[next].mValue, t);
		}
	}
	return (*key.rbegin()).mValue;
}

std::vector<Matrix4> Animation::UpdatePoseAtTime(Skeleton* skeleton, float time)
{
	std::vector<Matrix4> poses;// (skeleton->GetJoints().size());
	// ローカル行列
	for (const Joint& joint : skeleton->GetJoints())
	{
		if (auto it = mNodeAnimations.find(joint.mName); it != mNodeAnimations.end())
		{
			const NodeAnimation& nodeAnim = (*it).second;
			auto scale = UpdateKeyframeAtTime(nodeAnim.mScale, time);
			auto rotate = UpdateKeyframeAtTime(nodeAnim.mRotate, time);
			auto translate = UpdateKeyframeAtTime(nodeAnim.mTranslate, time);
			poses.emplace_back(Matrix4::CreateAffine(scale, rotate, translate));
		}
		else
		{
			poses.emplace_back(joint.mLocal);
		}
	}
	for (const Joint& joint : skeleton->GetJoints())
	{
		// スケルトンスペースでの行列
		if (joint.mParent)
		{
			poses[joint.mIndex] = poses[joint.mIndex] * poses[*joint.mParent];
		}
		else
		{
			poses[joint.mIndex] = poses[joint.mIndex];
		}
	}
	// スケルトンスペースでの行列
	/*for (const Joint& joint : skeleton->GetJoints())
	{
		if (joint.mParent)
		{
			poses[joint.mIndex] *= poses[*joint.mParent];
		}
	}*/
	return poses;
}
