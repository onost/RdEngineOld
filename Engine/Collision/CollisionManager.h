#pragma once
#include "Collision.h"
#include "CollisionAttr.h"
#include <vector>

class Collider;

class CollisionManager
{
public:
	struct CollisionPair
	{
		Collider* mCollider1;
		Collider* mCollider2;
		CollisionInfo mInfo;
	};

	// すべてのコライダーをテスト
	void TestAllCollider();
	// レイキャスト
	bool Raycast(
		const Ray& ray, RaycastInfo& info, CollisionAttr attr = CollisionAttr::All);

	// コライダーを追加、削除
	void AddCollider(Collider* collider);
	void RemoveCollider(Collider* collider);

private:
	// StayPairに属しているか
	bool IsStayPair(Collider* a, Collider* b);
	// 全ペアに、あるコライダーが含まれているペアを削除
	void RemoveFromAllPairs(Collider* collider);

private:
	std::vector<Collider*> mColliders;
	// ペア
	std::vector<CollisionPair> mAllPairs;// 全ペア
	std::vector<CollisionPair> mEnterPairs;// Enter
	std::vector<CollisionPair> mStayPairs;// Stay
	std::vector<CollisionPair> mExitPairs;// Exit
};
