#include "Meteorite.h"
#include "Component/MeshRenderer.h"
#include "Component/SphereCollider.h"
#include "Graphics/Renderer.h"
#include "GravityBody.h"
#include "Scene/Scene.h"
#include "Random.h"
#include "MeteoriteChip.h"
#include "RdEngine.h"

Meteorite::Meteorite(Scene* scene)
	: Actor(scene)
	, mRandRot(Vector3::kZero)
{
	mName = "Meteorite";
	//mGravityBody = new GravityBody(this);
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Meteorite.gltf"));
	for (auto m : mr->GetModel()->GetMaterials())
	{
		m.second->SetIsShadowCast(false);
	}
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetAttribute(CollisionAttr::Enemies);// 味方
	sc->SetSphere({ {0.0f,0.0f,0.0f},2.0f });
	sc->SetResponse(CollisionAttr::All);

	mTransform->mScale *= 3.0f;

	mRandRot = Random::Rand(
		Vector3(-MyMath::k2Pi, -MyMath::k2Pi, -MyMath::k2Pi),
		Vector3(MyMath::k2Pi, MyMath::k2Pi, MyMath::k2Pi)) * 0.5f;
}

void Meteorite::ActorUpdate(float deltaTime)
{
	mTransform->mPosition += mVelocity;
	mTransform->mRotation *= ToQuaternion(
		Matrix4::CreateRotateX(mRandRot.x * deltaTime) *
		Matrix4::CreateRotateY(mRandRot.y * deltaTime) *
		Matrix4::CreateRotateZ(mRandRot.z * deltaTime));

	auto light = mScene->GetRenderer()->GetLightManager();
	auto normal = Normalize(-mVelocity);
	light->SetCircleShadowDirection(mShadowIndex, -normal);
	light->SetCircleShadowPosition(mShadowIndex, mTransform->GetWorld().GetTranslation() + normal * 10.0f);
	light->SetCircleShadowRadius(mShadowIndex, 50.0f);
	light->SetCircleShadowIntensity(mShadowIndex, 0.3f);
	light->SetCircleShadowDecay(mShadowIndex, 0.8f);
	light->SetCircleShadowInner(mShadowIndex, 0.0f);
	light->SetCircleShadowOuter(mShadowIndex, 16.0f);
}

void Meteorite::ActorOnCollisionEnter(Actor* owner, CollisionInfo*)
{
	if (owner->GetName() != mName)
	{
		mState = State::kDead;
		auto light = mScene->GetRenderer()->GetLightManager();
		light->SetCircleShadowIntensity(mShadowIndex, 0.0f);

		auto audio = gEngine->GetAudioSystem();
		auto data = audio->Load("Assets/Audio/Meteo.wav");
		audio->Play(data);

		for (uint32_t i = 0; i < 3; ++i)
		{
			auto chip = new MeteoriteChip(mScene);
			chip->mTransform->mPosition = mTransform->mPosition;
			auto r = Random::Rand(
				Vector3(-MyMath::k2Pi, -MyMath::k2Pi, -MyMath::k2Pi),
				Vector3(MyMath::k2Pi, MyMath::k2Pi, MyMath::k2Pi)) * 0.5f;
			chip->mTransform->mRotation = ToQuaternion(
				Matrix4::CreateRotateX(r.x) *
				Matrix4::CreateRotateY(r.y) *
				Matrix4::CreateRotateZ(r.z));
			chip->mTransform->mScale = Vector3::kOne * 3.0f;
			chip->SetDownVec(Normalize(mVelocity) * 2.0f);

			Vector3 vel = Random::Rand(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));

			chip->SetVelocity(vel);
			chip->mTransform->UpdateWorld();
		}
	}
}
