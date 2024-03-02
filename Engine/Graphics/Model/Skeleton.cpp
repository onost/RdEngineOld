#include "Skeleton.h"

void SkeletonOld::ComputeInvBindPose()
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



void Skeleton::Update()
{
	for (Joint& joint : mJoints)
	{
		joint.mLocal = Matrix4::CreateAffine(
			joint.mTransform.mScale,
			joint.mTransform.mRotate,
			joint.mTransform.mTranslate);
		if (joint.mParent)
		{
			joint.mSkeletonSpaceMat = joint.mLocal * mJoints[*joint.mParent].mSkeletonSpaceMat;
		}
		else
		{
			joint.mSkeletonSpaceMat = joint.mLocal;
		}
	}
}

void Skeleton::ApplyAnimation(const Animation& animation, float time)
{
	for (Joint& joint : mJoints)
	{
		if (auto it = animation.mNodeAnimations.find(joint.mName); it != animation.mNodeAnimations.end())
		{
			const NodeAnimation& nodeAnim = (*it).second;
			joint.mTransform.mScale = CalcValue(nodeAnim.mScale, time);
			joint.mTransform.mRotate = CalcValue(nodeAnim.mRotate, time);
			joint.mTransform.mTranslate = CalcValue(nodeAnim.mTranslate, time);
		}
	}
}
