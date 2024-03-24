#pragma once
#include "Actor/Actor.h"

//class ParticleManager;
class ParticleRenderer;

// テストシーン用プレイヤー
class DemoPlayer : public Actor
{
public:
	DemoPlayer(Scene* scene);

	void ActorInput(const InputSystem::State& input) override;
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollisionStay(Actor* other, CollisionInfo* info) override;
	void ActorUpdateForDev() override;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const { return Type::DemoPlayer; }

private:
	float mRadius;
	// 速度
	Vector3 mVelocity;
	float mSpeed;
	// 回転
	float mRotVel;
	float mRotSpeed;
	// ジャンプ
	bool mIsGround;
	float mJumpPower;
	float mGravity;
	float mGroundDist;
	float mMaxGround;
	// 弾
	float mInitCooldown;
	float mCooldown;

	// 砂煙
	ParticleRenderer* mSandSmoke;
};
