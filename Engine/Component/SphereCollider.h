#pragma once
#include "Collider.h"
#include "Collision/Collision.h"

// 球コライダー
class SphereCollider : public Collider
{
public:
	SphereCollider(Actor* owner);
	~SphereCollider();
	void OnUpdateWorld() override;

	bool Dispatch(Collider* other, CollisionInfo& info) override;
	bool IsCollision(SphereCollider* other, CollisionInfo& info) override;
	bool IsCollision(BoxCollider* other, CollisionInfo& info) override;
	//bool IsCollision(MeshCollider* other, CollisionInfo& info) override;

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
	Type GetType() const override { return Type::SphereCollider; }
	void SetSphere(const Sphere& sphere) { mLocal = sphere; }
	const Sphere& GetSphere() const { return mWorld; }

private:
	Sphere mLocal;
	Sphere mWorld;
};
