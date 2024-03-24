#include "StarPieceManager.h"
#include "Component/CameraComponent.h"
#include "Component/SpriteRenderer.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"
#include "Loader/JsonLoader.h"
#include "RdEngine.h"

StarPieceManager::StarPieceManager(Scene* scene)
	: Actor(scene)
	, mStarPieceCount(0)
	, mTimer(0.0f)
{
	mName = "StarPieceManager";

	for (uint32_t i = 0; i < kStarPieceMax; ++i)
	{
		Actor* actor = new Actor(mScene);
		actor->SetName(std::format("StarPieceUI0{}", i + 1));
		actor->SetParent(this);
		// スプライト
		mStarPieceSprites[i] = new SpriteRenderer(actor);
		std::string name = std::format("Assets/Texture/StarChip_{}.png", i + 1);
		mStarPieceSprites[i]->SetTexture(mScene->GetRenderer()->GetTexture(name));
		mStarPieceSprites[i]->SetIsVisible(false);
	}
}

void StarPieceManager::ActorUpdate(float deltaTime)
{
	const float kTimeMax = 2.5f;
	if (mStarPieceCount >= kStarPieceMax &&
		mTimer < kTimeMax)
	{
		if (mTimer >= 0.9f)
		{
			auto actor = mScene->GetActor("GoalBarrier");
			if (actor)
			{
				actor->SetState(State::kDead);
				auto audio = gEngine->GetAudioSystem();
				auto data = audio->Load("Assets/Audio/Open.wav");
				audio->Play(data);
			}
		}

		mTimer += deltaTime;
		if (mTimer >= kTimeMax)
		{
			mPlayerCam->SetMainCamera();
		}
	}
	if (!mPlayerCam)
	{
		auto actor = mScene->GetActor("PlayerCamera");
		auto component = actor->GetComponent(Component::Type::CameraComponent);
		if (component)
		{
			mPlayerCam = dynamic_cast<CameraComponent*>(component);
		}
	}
	if (!mBreakCam)
	{
		auto actor = mScene->GetActor("BreakCamera");
		auto component = actor->GetComponent(Component::Type::CameraComponent);
		if (component)
		{
			mBreakCam = dynamic_cast<CameraComponent*>(component);
		}
	}
}

void StarPieceManager::GetStarPiece()
{
	mStarPieceSprites[mStarPieceCount]->SetIsVisible(true);
	++mStarPieceCount;
	if (mStarPieceCount >= kStarPieceMax)
	{
		mBreakCam->SetMainCamera();
		JsonLoader::LoadPrefab(mScene, "Assets/Prefab/GlassChip.rdpr");
	}
}
