#include "CollisionManager.h"
#include "Actor/Actor.h"
#include "Component/BoxCollider.h"
#include "Component/MeshCollider.h"
#include "Component/SphereCollider.h"

// すべてのコライダーをテスト
void CollisionManager::TestAllCollider()
{
	// StayかExitか
	mStayPairs.clear();
	mExitPairs.clear();
	for (auto& pair : mAllPairs)
	{
		Collider* a = pair.mCollider1;
		Collider* b = pair.mCollider2;
		// Aの属性とBの反応属性が一致かつBの属性とAの反応属性が一致
		if ((a->mAttribute & b->mResponse) && (b->mAttribute & a->mResponse))
		{
			CollisionInfo info = {};
			if (a->Dispatch(b, info))
			{
				// まだ衝突している
				pair.mInfo = info;
				mStayPairs.emplace_back(pair);// Stay
			}
			else
			{
				// もう衝突していない
				pair.mInfo = info;
				mExitPairs.emplace_back(pair);// Exit
			}
		}
	}

	// すべてのコライダーをテスト
	mAllPairs.clear();
	mEnterPairs.clear();
	for (size_t i = 0; i < mColliders.size(); ++i)
	{
		for (size_t j = i + 1; j < mColliders.size(); ++j)
		{
			Collider* a = mColliders[i];
			Collider* b = mColliders[j];
			// Aの属性とBの反応属性が一致かつBの属性とAの反応属性が一致
			if ((a->mAttribute & b->mResponse) && (b->mAttribute & a->mResponse))
			{
				CollisionInfo info = {};
				if (a->Dispatch(b, info))
				{
					CollisionPair pair = {};
					pair.mCollider1 = a;
					pair.mCollider2 = b;
					pair.mInfo = info;
					// 追加
					mAllPairs.emplace_back(pair);
					// StayではなかったらEnter
					if (!IsStayPair(a, b))
					{
						mEnterPairs.emplace_back(pair);
					}
				}
			}
		}
	}

	// Enter
	for (auto& pair : mEnterPairs)
	{
		Actor* owner1 = pair.mCollider1->GetOwner();
		Actor* owner2 = pair.mCollider2->GetOwner();
		// どちらかのコライダーがトリガーであればOnTriggerEnter
		if (pair.mCollider1->mIsTrigger || pair.mCollider2->mIsTrigger)
		{
			owner1->OnTriggerEnter(owner2);
			owner2->OnTriggerEnter(owner1);
		}
		// どちらもトリガーでなければOnCollisionEnter
		else
		{
			owner1->OnCollisionEnter(owner2, &pair.mInfo);
			CollisionInfo info = pair.mInfo;
			info.mNormal *= -1.0f;// 反転
			owner2->OnCollisionEnter(owner1, &info);
		}
	}
	// Stay
	for (auto& pair : mStayPairs)
	{
		Actor* owner1 = pair.mCollider1->GetOwner();
		Actor* owner2 = pair.mCollider2->GetOwner();
		if (pair.mCollider1->mIsTrigger || pair.mCollider2->mIsTrigger)
		{
			owner1->OnTriggerStay(owner2);
			owner2->OnTriggerStay(owner1);
		}
		else
		{
			owner1->OnCollisionStay(owner2, &pair.mInfo);
			CollisionInfo info = pair.mInfo;
			info.mNormal *= -1.0f;
			owner2->OnCollisionStay(owner1, &info);
		}
	}
	// Exit
	for (auto& pair : mExitPairs)
	{
		Actor* owner1 = pair.mCollider1->GetOwner();
		Actor* owner2 = pair.mCollider2->GetOwner();
		if (pair.mCollider1->mIsTrigger || pair.mCollider2->mIsTrigger)
		{
			owner1->OnTriggerExit(owner2);
			owner2->OnTriggerExit(owner1);
		}
		else
		{
			owner1->OnCollisionExit(owner2, &pair.mInfo);
			CollisionInfo info = pair.mInfo;
			info.mNormal *= -1.0f;
			owner2->OnCollisionExit(owner1, &info);
		}
	}
}

// レイキャスト
bool CollisionManager::Raycast(const Ray& ray, RaycastInfo& info, CollisionAttr attr)
{
	bool isHit = false;
	float minT = MyMath::kInfinity;
	for (auto& c : mColliders)
	{
		if (!(c->mAttribute & attr) ||
			c->mIsTrigger)//
		{
			continue;
		}
		float t;
		Vector3 normal;
		// 球
		if (c->GetType() == Component::Type::SphereCollider)
		{
			SphereCollider* sphere = dynamic_cast<SphereCollider*>(c);
			if (Intersect(ray, sphere->GetSphere(), t, normal))
			{
				if (t < minT)
				{
					isHit = true;
					minT = t;
					// Info
					info.mPoint = ray.GetPoint(t);
					info.mNormal = normal;
					info.mCollider = sphere;
					info.mActor = sphere->GetOwner();
				}
			}
		}
		// ボックス
		if (c->GetType() == Component::Type::BoxCollider)
		{
			BoxCollider* box = dynamic_cast<BoxCollider*>(c);
			if (Intersect(ray, box->GetBox(), t, normal))
			{
				if (t < minT)
				{
					isHit = true;
					minT = t;
					// Info
					info.mPoint = ray.GetPoint(t);
					info.mNormal = normal;
					info.mCollider = box;
					info.mActor = box->GetOwner();
				}
			}
		}
		// メッシュ
		if (c->GetType() == Component::Type::MeshCollider)
		{
			MeshCollider* mesh = dynamic_cast<MeshCollider*>(c);
			if (mesh->Raycast(ray, t, normal))
			{
				if (t < minT)
				{
					isHit = true;
					minT = t;
					// Info
					info.mPoint = ray.GetPoint(t);
					info.mNormal = normal;
					info.mCollider = mesh;
					info.mActor = mesh->GetOwner();
				}
			}
		}
	}
	return isHit;
}

// コライダーを追加
void CollisionManager::AddCollider(Collider* collider)
{
	mColliders.emplace_back(collider);
}

// コライダーを削除
void CollisionManager::RemoveCollider(Collider* collider)
{
	auto it = std::find(mColliders.begin(), mColliders.end(), collider);
	if (it != mColliders.end())
	{
		std::iter_swap(it, mColliders.end() - 1);
		mColliders.pop_back();
	}
	RemoveFromAllPairs(collider);
}

bool CollisionManager::IsStayPair(Collider* a, Collider* b)
{
	for (auto& pair : mStayPairs)
	{
		if (pair.mCollider1 == a &&
			pair.mCollider2 == b)
		{
			return true;
		}
	}
	return false;
}

// 全ペアに、あるコライダーが含まれているペアを削除
void CollisionManager::RemoveFromAllPairs(Collider* collider)
{
	auto it = mAllPairs.begin();
	for (; it != mAllPairs.end();)
	{
		if (it->mCollider1 == collider || it->mCollider2 == collider)
		{
			it = mAllPairs.erase(it);
		}
		else
		{
			++it;
		}
	}
}
