#pragma once
#include "Collision.h"
#include "CollisionAttribute.h"
#include "Component/Collider.h"
#include <unordered_set>
#include <vector>

// 衝突ペア
struct CollisionPair
{
	Collider* mCollider1;
	Collider* mCollider2;
	CollisionInfo mInfo;

	struct Hash
	{
		size_t operator()(const CollisionPair& pair) const
		{
			size_t h1 = std::hash<Collider*>()(pair.mCollider1);
			size_t h2 = std::hash<Collider*>()(pair.mCollider2);
			return h1 ^ (h2 << 1);
		}
	};

	struct Equal
	{
		size_t operator()(const CollisionPair& a, const CollisionPair& b) const
		{
			return
				(a.mCollider1 == b.mCollider1 && a.mCollider2 == b.mCollider2) ||
				(a.mCollider1 == b.mCollider2 && a.mCollider2 == b.mCollider1);
		}
	};
};

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	// すべてのコライダーをテスト
	void ProcessCollision();

	// レイキャスト
	bool Raycast(
		const Ray& ray, RaycastInfo& info,
		CollisionAttribute attribute = CollisionAttribute::kAll);

	// コライダーを追加、削除
	void AddCollider(std::shared_ptr<Collider> collider);
	void RemoveCollider(std::shared_ptr<Collider> collider);

private:
	std::vector<std::shared_ptr<Collider>> mColliders;
	// 衝突ペア
	std::unordered_set<CollisionPair, CollisionPair::Hash, CollisionPair::Equal> mCurrPairs;
	std::vector<CollisionPair> mEnterPairs;
	std::vector<CollisionPair> mStayPairs;
	std::vector<CollisionPair> mExitPairs;
};
