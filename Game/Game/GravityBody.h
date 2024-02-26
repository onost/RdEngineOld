#pragma once
#include "Component/Component.h"

class Attractor;

// 重力の影響を受けるオブジェクト
class GravityBody : public Component
{
public:
	GravityBody(Actor* owner);
	~GravityBody();
	void Update(float deltaTime) override;

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
	void SetAttractor(Attractor* attractor) { mAttractor = attractor; }

private:
	Attractor* mAttractor;
	float mMass;// 質量
};
