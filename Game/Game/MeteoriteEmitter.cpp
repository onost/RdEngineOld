#include "MeteoriteEmitter.h"
#include "Meteorite.h"
#include "Random.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Graphics/LightManager.h"
#include "Scene/Scene.h"

MeteoriteEmitter::MeteoriteEmitter(Scene* scene)
	: Actor(scene)
	, mFrequency(1.0f)
	, mInitFreq(1.0f)
	, mRadius(80.0f)
	, mShadowIndex(1)
{

}

void MeteoriteEmitter::ActorUpdate(float deltaTime)
{
	mFrequency = MyMath::Max(mFrequency - deltaTime, 0.0f);
	if (mFrequency <= 0.0f)
	{
		Emit();
		mFrequency = mInitFreq;
	}
}

// 隕石を発生させる
void MeteoriteEmitter::Emit()
{
	auto meteorite = new Meteorite(mScene);
	auto player = mScene->GetActor("Player");



	/*float theta = Random::Rand(0.0f, MyMath::k2Pi);
	float phi = Random::Rand(0.0f, MyMath::k2Pi);
	Vector3 pos = mTransform->mPosition +
		Vector3(
			mRadius * cosf(theta) * cosf(phi),
			mRadius * cosf(theta) * sinf(phi),
			mRadius * sin(theta));*/
	if (player)
	{
		//float up = 80.0f;
		Vector3 min = Vector3(-20.0f, 25.0f, -20.0f);
		Vector3 max = Vector3(20.0f, 25.0f, 60.0f);
		Vector3 p = Random::Rand(min, max) * player->mTransform->mRotation;

		meteorite->mTransform->mPosition = player->mTransform->mPosition + p;
		meteorite->SetVelocity(Normalize(mTransform->mPosition - meteorite->mTransform->mPosition) * 0.18f);
		meteorite->SetShadowIndex(mShadowIndex++);
		if (mShadowIndex >= LightManager::kCircleShadowCount)
		{
			// 0はプレイヤーが使う
			mShadowIndex = 1;
		}
	}
}

void MeteoriteEmitter::ActorUpdateForDev()
{
	ImGui::Text("This is meteorite emitter!");
	ImGui::DragFloat("Radius", &mRadius, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Frequency", &mInitFreq, 0.01f, 0.0f, 5.0f);
}

void MeteoriteEmitter::ActorRenderForDev(Primitive* prim)
{
	prim->DrawSphere(mTransform->mPosition, mRadius, Color::kWhite);
}
