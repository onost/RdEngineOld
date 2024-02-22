#pragma once
#include "Component/Component.h"

class GravityAttractor : public Component
{
public:
	GravityAttractor(Actor* owner, uint32_t order = 100);
	~GravityAttractor();

	// アクターを重力で引っ張る
	void Attract(Actor* actor);

	// ==================================================
	// json
	// ==================================================
	virtual void Load(const nlohmann::json& json);
	virtual void Save(nlohmann::json& json);

	// ==================================================
	// 開発用
	// ==================================================
	virtual void UpdateForDev();
	virtual void RenderForDev(Primitive* prim);

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const { return Type::GravityAttractor; }

private:
	Vector3 mCenter;// 中心
	float mRadius;// 半径
	float mGravity;// 重力
	float mMass;// 質量
	float mIsInvSquareLaw;// 逆2乗則
};
