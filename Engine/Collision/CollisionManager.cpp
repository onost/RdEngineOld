#include "CollisionManager.h"
#include "Actor/Actor.h"
#include "Component/BoxCollider.h"
#include "Component/MeshCollider.h"
#include "Component/SphereCollider.h"

// すべてのコライダーをテスト
void CollisionManager::TestAllCollider(
	std::function<void(Actor*, Actor*, CollisionInfo*)> func)
{
	// 色を白へ
	/*for (auto& c : mColliders)
	{
		c->SetColor(Color::kWhite);
	}*/

	for (size_t i = 0; i < mColliders.size(); ++i)
	{
		for (size_t j = i + 1; j < mColliders.size(); ++j)
		{
			Collider* a = mColliders[i];
			Collider* b = mColliders[j];
			if ((a->mAttr & b->mResponseAttr) &&
				(b->mAttr & a->mResponseAttr))
			{
				CollisionInfo info = {};
				if (a->Dispatch(b, info))
				{
					func(a->GetOwner(), b->GetOwner(), &info);

					if (a->mIsTrigger || b->mIsTrigger)
					{
						info.mNormal = Vector3::kZero;
					}

					//a->SetColor(Color::kRed);
					//b->SetColor(Color::kRed);
				}
			}
		}
	}
}

// レイキャスト
bool CollisionManager::Raycast(
	const Ray& ray, RaycastHit& info, Collider::Attribute attr)
{
	bool isHit = false;
	float minT = MyMath::kInfinity;
	for (auto& c : mColliders)
	{
		if (!(c->mAttr & attr))
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
}
