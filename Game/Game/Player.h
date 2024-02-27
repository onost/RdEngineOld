#pragma once
#include "Actor/Actor.h"

class ParticleRenderer;
class Attractor;
class GravityBody;

// プレイヤー
class Player : public Actor
{
public:
	Player(Scene* scene);

	void ActorInput(const Input::State& input) override;
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollision(Actor* other, CollisionInfo* info) override;
	void ActorOnTrigger(Actor* other) override;
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

	// 重力
	GravityBody* mGravityBody;
};
