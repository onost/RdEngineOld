#pragma once
#include "Component.h"
#include "Graphics/Model/Animation.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>

class Animation;

class Animator : public Component
{
public:
	Animator(Actor* owner);
	virtual ~Animator();
	void Update(float deltaTime) override;
	void OnUpdateWorld() override;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::Animator; }
	void SetAnimation(Animation* anim);

private:
	Animation* mAnimation;
	float mCurrTime;
	bool mIsLoop;
	// 開発用
	std::string mAnimName;
};
