#include "Skeleton.h"
#include "Graphics/Model/Animation.h"

void Skeleton::Update()
{
	for (Joint& joint : mJoints)
	{
		// ローカル行列
		joint.mLocal = Matrix4::CreateAffine(
			joint.mTransform.mScale,
			joint.mTransform.mRotate,
			joint.mTransform.mTranslate);
		// スケルトンスペースでの行列
		if (joint.mParent)
		{
			joint.mSkelSpaceMat = joint.mLocal * mJoints[*joint.mParent].mSkelSpaceMat;
		}
		else
		{
			joint.mSkelSpaceMat = joint.mLocal;
		}
	}
}

/*void Skeleton::ApplyAnimation(const Animation* anim, float time)
{
	for (Joint& joint : mJoints)
	{
		auto nodeAnims = anim->GetNodeAnimations();
		if (auto it = nodeAnims.find(joint.mName); it != nodeAnims.end())
		{
			const NodeAnimation& nodeAnim = (*it).second;
			joint.mTransform.mScale = UpdateKeyframeAtTime(nodeAnim.mScale, time);
			joint.mTransform.mRotate = UpdateKeyframeAtTime(nodeAnim.mRotate, time);
			joint.mTransform.mTranslate = UpdateKeyframeAtTime(nodeAnim.mTranslate, time);
		}
	}
}*/
