#pragma once
#include "Component/Collider.h"
#include <functional>

// 衝突管理
class CollisionManager
{
public:
	struct CollisionPair
	{
		Collider* a;
		Collider* b;
		CollisionInfo info;
	};

	//bool Initialize();
	//void Terminate();

	// すべてのコライダーをテスト
	void TestAllCollider();
	// レイキャスト
	bool Raycast(const Ray& ray, RaycastInfo& info,
		Collider::Attribute attribute = Collider::kAll);

	// コライダーを追加、削除
	void AddCollider(Collider* collider);
	void RemoveCollider(Collider* collider);

private:
	bool IsInStayPair(Collider* a, Collider* b)
	{
		for (auto& pair : mStayPairs)
		{
			if (pair.a == a && pair.b == b)
			{
				return true;
			}
		}
		return false;
	}

private:
	std::vector<Collider*> mColliders;
	// ペア
	std::vector<CollisionPair> mCollisionPairs;// 全ペア
	std::vector<CollisionPair> mEnterPairs;// Enter
	std::vector<CollisionPair> mStayPairs;// Stay
	std::vector<CollisionPair> mExitPairs;// Exit
};
