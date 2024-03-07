#include "StarPiece.h"
#include "Component/Animator.h"
#include "Component/MeshRenderer.h"
#include "Graphics/Renderer.h"
#include "GravityBody.h"
#include "Scene/Scene.h"

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

void StarPiece::ActorOnTriggerEnter(Actor* other)
{
	// ゴール！
	if (other->GetName() == "Player")
	{
		mAnimator->SetAnimation(mScene->GetRenderer()->GetAnimation("StarPiece.gltf/CubeAction.002"));
	}
}
