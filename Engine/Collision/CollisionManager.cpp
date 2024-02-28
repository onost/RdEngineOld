#include "CollisionManager.h"
#include "Actor/Actor.h"
#include "Component/BoxCollider.h"
#include "Component/MeshCollider.h"
#include "Component/SphereCollider.h"

// すべてのコライダーをテスト
void CollisionManager::TestAllCollider()
{
	// 色を白へ
	/*for (auto& c : mColliders)
	{
		c->SetColor(Color::kWhite);
	}*/

	mStayPairs.clear();
	mExitPairs.clear();
	for (auto& pair : mCollisionPairs)
	{
		Collider* a = pair.a;
		Collider* b = pair.b;
		if ((a->mAttribute & b->mResponse) &&
			(b->mAttribute & a->mResponse))
		{
			CollisionInfo info = {};
			if (a->Dispatch(b, info))
			{
				pair.info = info;
				mStayPairs.emplace_back(pair);
			}
			else
			{
				pair.info = info;
				mExitPairs.emplace_back(pair);
			}
		}
	}

	mCollisionPairs.clear();
	mEnterPairs.clear();
	for (size_t i = 0; i < mColliders.size(); ++i)
	{
		for (size_t j = i + 1; j < mColliders.size(); ++j)
		{
			Collider* a = mColliders[i];
			Collider* b = mColliders[j];
			if ((a->mAttribute & b->mResponse) &&
				(b->mAttribute & a->mResponse))
			{
				CollisionInfo info = {};
				if (a->Dispatch(b, info))
				{
					CollisionPair pair = {};
					pair.a = a;
					pair.b = b;
					pair.info = info;

					mCollisionPairs.emplace_back(pair);
					if (!IsInStayPair(a, b))
					{
						mEnterPairs.emplace_back(pair);
					}

					//a->SetColor(Color::kRed);
					//b->SetColor(Color::kRed);
				}
			}
		}
	}

	// Enter
	for (auto& pair : mEnterPairs)
	{
		Actor* aOwner = pair.a->GetOwner();
		Actor* bOwner = pair.b->GetOwner();
		if (pair.a->mIsTrigger || pair.b->mIsTrigger)
		{
			aOwner->OnTriggerEnter(bOwner);
			bOwner->OnTriggerEnter(aOwner);
		}
		else
		{
			aOwner->OnCollisionEnter(bOwner, &pair.info);
			CollisionInfo i = pair.info;
			i.mNormal *= -1.0f;
			bOwner->OnCollisionEnter(aOwner, &i);
		}
	}
	// Stay
	for (auto& pair : mStayPairs)
	{
		Actor* aOwner = pair.a->GetOwner();
		Actor* bOwner = pair.b->GetOwner();
		if (pair.a->mIsTrigger || pair.b->mIsTrigger)
		{
			aOwner->OnTriggerStay(bOwner);
			bOwner->OnTriggerStay(aOwner);
		}
		else
		{
			aOwner->OnCollisionStay(bOwner, &pair.info);
			CollisionInfo i = pair.info;
			i.mNormal *= -1.0f;
			bOwner->OnCollisionStay(aOwner, &i);
		}
	}
	// Exit
	for (auto& pair : mExitPairs)
	{
		Actor* aOwner = pair.a->GetOwner();
		Actor* bOwner = pair.b->GetOwner();
		if (pair.a->mIsTrigger || pair.b->mIsTrigger)
		{
			aOwner->OnTriggerExit(bOwner);
			bOwner->OnTriggerExit(aOwner);
		}
		else
		{
			aOwner->OnCollisionExit(bOwner, &pair.info);
			CollisionInfo i = pair.info;
			i.mNormal *= -1.0f;
			bOwner->OnCollisionExit(aOwner, &i);
		}
	}
}

// レイキャスト
bool CollisionManager::Raycast(
	const Ray& ray, RaycastInfo& info, Collider::Attribute attribute)
{
	bool isHit = false;
	float minT = MyMath::kInfinity;
	for (auto& c : mColliders)
	{
		if (!(c->mAttribute & attribute))
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
	{
		auto it = std::find(mColliders.begin(), mColliders.end(), collider);
		if (it != mColliders.end())
		{
			std::iter_swap(it, mColliders.end() - 1);
			mColliders.pop_back();
		}
	}
	{
		auto it = mCollisionPairs.begin();
		for (; it != mCollisionPairs.end();)
		{
			if (it->a == collider || it->b == collider)
			{
				it = mCollisionPairs.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
