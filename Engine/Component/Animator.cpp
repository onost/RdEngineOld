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
			mCurrTime = std::fmod(mCurrTime, mAnimation->mDuration);
		}
	}
}

void Animator::OnUpdateWorld()
{
	if (mAnimation)
	{
		NodeAnimation& nodeAnim = mAnimation->mNodeAnimations.begin()->second;//
		// ローカル行列を作成
		Vector3 scale = CalcValue(nodeAnim.mScale, mCurrTime);
		Quaternion rotate = CalcValue(nodeAnim.mRotate, mCurrTime);
		Vector3 translate = CalcValue(nodeAnim.mTranslate, mCurrTime);
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
			mAnimName = mAnimation->mName;
		}
	}
}

void Animator::Save(nlohmann::json& json)
{
	Component::Save(json);

	if (mAnimation)
	{
		JsonHelper::SetString(json, "Animation", mAnimation->mName);
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
				mAnimName = mAnimation->mName;
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
