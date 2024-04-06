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

Vector3 UpdateKeyframeAtTime(
	const std::vector<Keyframe<Vector3>>& curr,
	const std::vector<Keyframe<Vector3>>& next,
	float time, float nextTime)
{
	MY_ASSERT(!curr.empty());
	MY_ASSERT(!next.empty());
	if (curr.size() == 1 || time <= curr[0].mTime)
	{
		return curr[0].mValue;
	}
	if (next.size() == 1 || time <= next[0].mTime)
	{
		return curr[0].mValue;
	}

	Vector3 a = Vector3::kZero, b = Vector3::kZero;
	float aTime = 0.0f, bTime = 0.0f;
	for (uint32_t c = 0; c < curr.size() - 1; ++c)
	{
		uint32_t n = c + 1;
		if (curr[c].mTime <= time &&
			curr[n].mTime >= time)
		{
			a = curr[c].mValue;
			aTime = curr[c].mTime;
			/*float t = (time - curr[c].mTime) / (curr[next].mTime - curr[c].mTime);
			return MyMath::Lerp(curr[c].mValue, curr[next].mValue, t);*/
		}
	}
	for (uint32_t c = 0; c < next.size() - 1; ++c)
	{
		uint32_t n = c + 1;
		if (next[c].mTime <= nextTime &&
			next[n].mTime >= nextTime)
		{
			a = next[n].mValue;
			bTime = next[n].mTime;
		}
	}
	float t = (time - aTime) / (bTime - aTime);
	return MyMath::Lerp(a, b, t);
	//return (*key.rbegin()).mValue;
}

Quaternion UpdateKeyframeAtTime(
	const std::vector<Keyframe<Quaternion>>& curr,
	const std::vector<Keyframe<Quaternion>>& next,
	float time, float nextTime)
{
	MY_ASSERT(!curr.empty());
	MY_ASSERT(!next.empty());
	if (curr.size() == 1 || time <= curr[0].mTime)
	{
		return curr[0].mValue;
	}
	if (next.size() == 1 || time <= next[0].mTime)
	{
		return curr[0].mValue;
	}

	Quaternion a, b;
	float aTime = 0.0f, bTime = 0.0f;
	for (uint32_t c = 0; c < curr.size() - 1; ++c)
	{
		uint32_t n = c + 1;
		if (curr[c].mTime <= time &&
			curr[n].mTime >= time)
		{
			a = curr[c].mValue;
			aTime = curr[c].mTime;
			/*float t = (time - curr[c].mTime) / (curr[next].mTime - curr[c].mTime);
			return MyMath::Lerp(curr[c].mValue, curr[next].mValue, t);*/
		}
	}
	for (uint32_t c = 0; c < next.size() - 1; ++c)
	{
		uint32_t n = c + 1;
		if (next[c].mTime <= nextTime &&
			next[n].mTime >= nextTime)
		{
			a = next[n].mValue;
			bTime = next[n].mTime;
		}
	}
	float t = (time - aTime) / (bTime - aTime);
	return Slerp(a, b, t);
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

std::vector<Matrix4> Animation::UpdatePoseAtTime(Skeleton* skeleton, Animation* next, float time, float nextTime, float t)
{
	std::vector<Matrix4> poses;// (skeleton->GetJoints().size());
	// ローカル行列
	//for (const Joint& joint : skeleton->GetJoints())
	auto joints = skeleton->GetJoints();
	for (uint32_t i = 0; i < joints.size(); ++i)
	{
		if (auto it = mNodeAnimations.find(joints[i].mName); it != mNodeAnimations.end())
		{
			const NodeAnimation& nodeAnim = (*it).second;
			auto scale1 = UpdateKeyframeAtTime(nodeAnim.mScale, time);
			auto rotate1 = UpdateKeyframeAtTime(nodeAnim.mRotate, time);
			auto translate1 = UpdateKeyframeAtTime(nodeAnim.mTranslate, time);
			const NodeAnimation& nextAnim = (*next->mNodeAnimations.find(joints[i].mName)).second;
			auto scale2 = UpdateKeyframeAtTime(nextAnim.mScale, nextTime);
			auto rotate2 = UpdateKeyframeAtTime(nextAnim.mRotate, nextTime);
			auto translate2 = UpdateKeyframeAtTime(nextAnim.mTranslate, nextTime);
			scale1 = MyMath::Lerp(scale1, scale2, t);
			rotate1 = Slerp(rotate1, rotate2, t);
			translate1 = MyMath::Lerp(translate1, translate2, t);
 			poses.emplace_back(Matrix4::CreateAffine(scale1, rotate1, translate1));
		}
		else
		{
			poses.emplace_back(joints[i].mLocal);
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
