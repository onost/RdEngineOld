#pragma once
#include "Actor/Actor.h"

// プレイヤー
class Player : public Actor
{
public:
	Player(Scene* scene);

	void ActorInput(const Input::State& input) override;
	void ActorUpdate(float deltaTime) override;
	void OnCollision(Actor* other, CollisionInfo* info) override;
	void ActorUpdateForDev() override;

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
	float mGravityPower;
	Vector3 mGravityDir;
	float mGroundDist;

	float mMaxGround;
	Vector3 mActualNorm;
	Vector3 mIdealNorm;
};
