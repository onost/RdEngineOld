#include "Animation.h"
#include "Skeleton.h"
#include "Helper/MyAssert.h"

std::vector<Matrix4> Animation::GetPoseAtTime(Skeleton* skeleton, float time)
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
