#include "MeteoriteChip.h"
#include "Component/MeshRenderer.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"
#include "Random.h"

MeteoriteChip::MeteoriteChip(Scene* scene)
	: Actor(scene)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("MeteoriteChip.obj"));

	mRandRot = Random::Rand(
		Vector3(-MyMath::k2Pi, -MyMath::k2Pi, -MyMath::k2Pi),
		Vector3(MyMath::k2Pi, MyMath::k2Pi, MyMath::k2Pi)) * 0.5f;
}

void MeteoriteChip::ActorUpdate(float deltaTime)
{
	mTransform->mRotation *= ToQuaternion(
		Matrix4::CreateRotateX(mRandRot.x * deltaTime) *
		Matrix4::CreateRotateY(mRandRot.y * deltaTime) *
		Matrix4::CreateRotateZ(mRandRot.z * deltaTime));
	mVelocity += mDownVec * deltaTime;
	mTransform->mPosition += mVelocity;
	mTransform->mScale -= Vector3::kOne * 6.0f * deltaTime;
	if (Length(mTransform->mScale) < 0.01f)
	{
		mState = State::kDead;
	}
}
