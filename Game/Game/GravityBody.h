#pragma once
#include "Component/Component.h"
#include <vector>

class Attractor;

// 重力の影響を受けるオブジェクト
class GravityBody : public Component
{
public:
	GravityBody(Actor* owner);
	~GravityBody();
	void Update(float deltaTime) override;
	void OnTriggerEnter(Actor* other) override;

	void AddForce(float force);

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;
	void RenderForDev(Primitive*) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::Attractor; }
	float GetMass() const { return mMass; }
	bool GetIsGround() const { return mIsGround; }
	const Vector3& GetNormal() const { return mNormal; }
	const Vector3& GetCurrNormal() const { return mCurrNormal; }
	void SetForce(float force) { mForce = force; }

private:
	Attractor* mAttractor;

	float mMass;// 質量
	float mForce;
	Vector3 mNormal;
	bool mIsGround;

	float mPushDist;
	float mGroundDist;

	//Vector3 mForce;
	Vector3 mCurrNormal;
};
