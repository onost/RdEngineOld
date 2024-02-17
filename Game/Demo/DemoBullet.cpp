#include "DemoBullet.h"
#include "Collision/CollisionManager.h"
#include "Component/MeshRenderer.h"
#include "Component/SphereCollider.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"

DemoBullet::DemoBullet(Scene* scene)
	: Actor(scene)
	, mLife(1.5f)// 1.5秒
	, mSpeed(150.0f)
{
	mName = "Bullet";
	mTransform->mScale = Vector3::kOne * 0.5f;
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));// とりま
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetSphere({ {0.0f,0.0f,0.0f},1.0f });
	sc->SetAttr(Collider::Allies);
	sc->AddResponseAttr(Collider::Terrain);
	sc->AddResponseAttr(Collider::Enemies);
}

void DemoBullet::ActorUpdate(float deltaTime)
{
	mLife -= deltaTime;
	if (mLife <= 0.0f)
	{
		mState = State::kDead;
	}

	// 移動
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation;
	mTransform->mPosition += f * mSpeed * deltaTime;
}

void DemoBullet::OnCollision(Actor* /*other*/, CollisionInfo* /*info*/)
{
	mState = State::kDead;
}
