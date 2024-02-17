#pragma once
#include "Core/ConstantBuffer.h"
#include "MeshRenderer.h"
#include "Graphics/Model/Skeleton.h"
#include <memory>

class Animation;
class Skeleton;

// スキンメッシュレンダラー
class SkinnedMeshRenderer : public MeshRenderer
{
public:
	SkinnedMeshRenderer(Actor* owner);
	~SkinnedMeshRenderer();

	void Update(float deltaTime) override;
	void Draw() override;
	// アニメーションを再生
	void PlayAnimation(Animation* anim);

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;
	void RenderForDev(Primitive* prim) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::SkinnedMeshRenderer; }
	Skeleton* GetSkeleton() const { return mSkeleton; }
	float GetCurrTime() const { return mCurrTime; }
	Animation* GetAnimation() const { return mCurrAnim; }
	void SetSkeleton(Skeleton* skeleton)
	{
		mSkeleton = skeleton;
		mSkelName = mSkeleton->GetName();
		mPoses.resize(mSkeleton->GetBoneCount());
	}

private:
	void ComputePose();

private:
	Skeleton* mSkeleton;
	// アニメーション
	float mCurrTime;
	Animation* mCurrAnim;
	//Animation* mNextAnim;
	std::unique_ptr<ConstantBuffer> mCBuff;
	//std::vector<Matrix4> mCurrPoses;

	// 開発用
	std::string mSkelName;
	std::string mAnimName;
	std::vector<Matrix4> mPoses;
};
