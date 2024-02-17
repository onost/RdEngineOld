#pragma once
#include "Collider.h"
#include "Collision/Collision.h"

// ボックスコライダー
class BoxCollider : public Collider
{
public:
	BoxCollider(Actor* owner);
	~BoxCollider();
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
	Type GetType() const override { return Type::BoxCollider; }
	void SetBox(const OBB& box) { mLocal = box; }
	const OBB& GetBox() const { return mWorld; }

private:
	OBB mLocal;
	OBB mWorld;
};
