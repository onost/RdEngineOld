#include "Skeleton.h"

void Skeleton::ComputeInvBindPose()
{
	mInvBindPoses.resize(GetBoneCount());
	// 1. グローバルバインドポーズ行列を計算
	mInvBindPoses[0] = mBones[0].mLocal.GetMatrix();
	for (uint32_t i = 1; i < mInvBindPoses.size(); ++i)
	{
		Matrix4 local = mBones[i].mLocal.GetMatrix();
		mInvBindPoses[i] = local * mInvBindPoses[mBones[i].mParent];
	}
	// 2. 逆行列を計算
	for (uint32_t i = 0; i < mInvBindPoses.size(); ++i)
	{
		mInvBindPoses[i] = Inverse(mInvBindPoses[i]);
	}
}
