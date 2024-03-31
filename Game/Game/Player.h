#pragma once
#include "Actor/Actor.h"
#include "Color.h"

class ParticleRenderer;
class Attractor;
class GravityBody;
class SkinnedMeshRenderer;
class SpriteRenderer;

// プレイヤー
class Player : public Actor
{
public:
	Player(Scene* scene);

	void Initialize() override;
	void ActorInput(const InputSystem::State& input) override;
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollisionStay(Actor* other, CollisionInfo* info) override;
	void ActorOnCollisionEnter(Actor* other, CollisionInfo* info) override;
	void ActorOnTriggerEnter(Actor* other) override;
	void ActorUpdateForDev() override;
	void ActorRenderForDev(Primitive* prim);

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const { return Type::Player; }

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
	float mGravityPow;
	float mGroundDist;
	float mMaxGround;

	uint32_t mHp;
	float mInvincibleTimer;
	SpriteRenderer* mDamage;
	SkinnedMeshRenderer* mRenderer;

	// 重力
	GravityBody* mGravityBody;

	Actor* mHpUI[3];
	SpriteRenderer* mGoalSprite;
	SpriteRenderer* mGameOverSprite;
};
