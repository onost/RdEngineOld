#pragma once
#include "Component/Component.h"

// アトラクター
class Attractor : public Component
{
public:
	Attractor(Actor* owner);
	~Attractor();

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

private:
	float mRadius;
	float mIsInvSquare;// 逆2乗の法則

};
