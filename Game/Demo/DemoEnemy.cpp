#include "DemoEnemy.h"
#include "Component/MeshRenderer.h"
#include "Component/BoxCollider.h"
#include "Collision/CollisionManager.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"

DemoEnemy::DemoEnemy(Scene* scene)
	: Actor(scene)
	, mHp(10)
	, mRadius(1.5f)
	, mSpeed(2.5f)
{
	mName = "Enemy";
	mTransform->mScale = Vector3::kOne * mRadius;
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));
	// コライダー
	auto bc = new BoxCollider(this);
	bc->SetAttribute(CollisionAttr::Enemies);// 敵
	bc->SetResponse(CollisionAttr::All);
	bc->SetBox({ {0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f} });
}

void DemoEnemy::ActorUpdate(float deltaTime)
{
	if (mHp <= 0)
	{
		mState = State::kDead;
	}

	auto player = mScene->GetActor("Demo Player");
	if (player)
	{
		Vector3 target = Vector3(
			player->mTransform->mPosition.x,
			mTransform->mPosition.y,
			player->mTransform->mPosition.z);
		auto lookAt = Matrix4::CreateLookAt(
			mTransform->mPosition,// eye
			target,// target
			Vector3(0.0f, 1.0f, 0.0f));// up
		mTransform->mRotation = Conjugate(ToQuaternion(lookAt));

		auto move = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation;
		mVelocity.x = move.x * mSpeed;
		mVelocity.z = move.z * mSpeed;
	}

	mVelocity.y += -1.0f;
	mTransform->mPosition += mVelocity * deltaTime;// 重力

	// 地面
	Ray ray = Ray(mTransform->mPosition, mTransform->mPosition + Vector3(0.0f, -1.0f, 0.0f));// 下へ
	RaycastInfo info = {};
	if (mScene->GetCollisionManager()->Raycast(ray, info, CollisionAttr::Terrain))
	{
		float dist = Length(info.mPoint - ray.mStart);
		if (dist <= mRadius + 0.1f)
		{
			mVelocity.y = 0.0f;
			// 押し戻し
			mTransform->mPosition = info.mPoint + Vector3(0.0f, mRadius, 0.0f);
		}
	}
}

// Collision
void DemoEnemy::ActorOnCollisionStay(Actor* /*other*/, CollisionInfo* info)
{
	// 押し戻し
	mTransform->mPosition = mTransform->mPosition + info->mNormal * info->mDepth;
	mTransform->UpdateWorld(mParent ? mParent->mTransform : nullptr);
}

// Trigger
void DemoEnemy::ActorOnTriggerEnter(Actor* other)
{
	if (other->GetName() == "Bullet")
	{
		--mHp;
	}
}
