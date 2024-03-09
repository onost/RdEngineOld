#include "Animator.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Helper/MyAssert.h"
#include "Scene/Scene.h"
#include <format>

Animator::Animator(Actor* owner)
	: Component(owner)
	, mAnimation(nullptr)
	, mCurrTime(0.0f)
	, mIsLoop(true)
	, mIsEnd(false)
	, mAnimName()
{

}

Animator::~Animator()
{

}

void Animator::Update(float deltaTime)
{
	if (mAnimation)
	{
		mCurrTime += deltaTime;
		if (mIsLoop)
		{
			mCurrTime = std::fmod(mCurrTime, mAnimation->GetDuration());
		}
		else if (mCurrTime >= mAnimation->GetDuration())
		{
			mIsEnd = true;
		}
	}
}

void Animator::OnUpdateWorld()
{
	if (mAnimation)
	{
		const NodeAnimation& nodeAnim = mAnimation->GetNodeAnimations().begin()->second;//
		// ローカル行列を作成
		Vector3 scale = UpdateKeyframeAtTime(nodeAnim.mScale, mCurrTime);
		Quaternion rotate = UpdateKeyframeAtTime(nodeAnim.mRotate, mCurrTime);
		Vector3 translate = UpdateKeyframeAtTime(nodeAnim.mTranslate, mCurrTime);
		Matrix4 local = Matrix4::CreateAffine(scale, rotate, translate);
		mOwner->mTransform->SetWorld(local * mOwner->mTransform->GetWorld());
	}
}

// ==================================================
// json
// ==================================================

void Animator::Load(const nlohmann::json& json)
{
	Component::Load(json);

	std::string animName;
	if (JsonHelper::GetString(json, "Animation", animName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		mAnimation = renderer->GetAnimation(animName);
		if (mAnimation)
		{
			mAnimName = mAnimation->GetName();
		}
	}
}

void Animator::Save(nlohmann::json& json)
{
	Component::Save(json);

	if (mAnimation)
	{
		JsonHelper::SetString(json, "Animation", mAnimation->GetName());
	}
}

// ==================================================
// 開発用
// ==================================================

void Animator::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Animator"))
	{
		ImGui::Text(std::format("Curr Time: {}", mCurrTime).c_str());
		ImGui::Checkbox("Is Loop", &mIsLoop);
		// Animation
		ImGui::InputText("Anim Name", &mAnimName);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("ANIM_PAYLOAD"))
			{
				mAnimation = *(Animation**)(payload->Data);
				mAnimName = mAnimation->GetName();
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load##Anim"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto anim = renderer->GetAnimation(mAnimName);
			if (anim)
			{
				mAnimation = anim;
			}
		}
		ImGui::TreePop();
	}
}

// ==================================================
// ヘルパー関数
// ==================================================
void Animator::SetAnimation(Animation* anim)
{
	if (!anim)
	{
		return;
	}
	mAnimation = anim;
	mCurrTime = 0.0f;
	//
	mAnimName = mAnimation->GetName();
}
