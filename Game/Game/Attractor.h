#pragma once
#include "Component/Component.h"

class GravityBody;

// アトラクター
class Attractor : public Component
{
public:
	Attractor(Actor* actor);
	~Attractor();

	// アクターを引っ張る
	void Attract(GravityBody* actor, float deltaTime);

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;
	//void RenderForDev(Primitive*) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::Attractor; }
	float GetRadius() const { return mRadius; }

private:
	float mRadius;// 半径
	Vector3 mCenter;// 中心
	float mGravity;// 重力
	bool mIsInvSquare;// 逆2乗の法則
	float mMass;// 質量
};
