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
	float GetCurrTime() const { return mCurrTime; }
	Animation* GetAnimation() const { return mCurrAnim; }
	void SetIsLoop(bool isLoop) { mIsLoop = isLoop; }

private:
	// アニメーション
	float mCurrTime;
	Animation* mCurrAnim;
	std::string mAnimName;
	bool mIsLoop;
};
