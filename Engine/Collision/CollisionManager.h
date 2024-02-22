#pragma once
#include "Component/Collider.h"
#include <functional>

// 衝突管理
class CollisionManager
{
public:
	//bool Initialize();
	//void Terminate();

	// すべてのコライダーをテスト
	void TestAllCollider(
		std::function<void(Actor*, Actor*, CollisionInfo*)> onCollision,
		std::function<void(Actor*, Actor*)> onTrigger);
	// レイキャスト
	bool Raycast(const Ray& ray, RaycastHit& info,
		Collider::Attribute attr = Collider::kAll);

	// コライダーを追加、削除
	void AddCollider(Collider* collider);
	void RemoveCollider(Collider* collider);

private:
	std::vector<Collider*> mColliders;
};
