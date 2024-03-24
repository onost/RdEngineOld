#include "StarPiece.h"
#include "Component/Animator.h"
#include "Component/MeshRenderer.h"
#include "Graphics/Renderer.h"
#include "GravityBody.h"
#include "Scene/Scene.h"
#include "StarPieceManager.h"

#include "RdEngine.h"

StarPiece::StarPiece(Scene* scene)
	: Actor(scene)
	, mAnimator(nullptr)
	, mGravityBody(nullptr)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("StarPiece.gltf"));
	// アニメータ
	mAnimator = new Animator(this);
	mAnimator->SetAnimation(mScene->GetRenderer()->GetAnimation("StarPiece.gltf/CubeAction.001"));
	// 重力
	mGravityBody = new GravityBody(this);
}

void StarPiece::ActorUpdate(float /*deltaTime*/)
{
	if (mAnimator->GetIsEnd())
	{
		mState = State::kDead;
	}
}

void StarPiece::ActorOnTriggerEnter(Actor* other)
{
	auto animName = "StarPiece.gltf/CubeAction.002";
	if (other->GetName() == "Player" &&
		mAnimator->GetAnimName() != animName)
	{
		mAnimator->SetAnimation(mScene->GetRenderer()->GetAnimation(animName));
		mAnimator->SetIsLoop(false);

		auto actor = mScene->GetActor("StarPieceManager");
		StarPieceManager* manager = dynamic_cast<StarPieceManager*>(actor);
		if (manager)
		{
			manager->GetStarPiece();
		}

		auto audio = gEngine->GetAudioSystem();
		auto data = audio->Load("Assets/Audio/ChipGet.wav");
		audio->Play(data);
	}
}
