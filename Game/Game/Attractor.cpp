#include "Attractor.h"
#include "Actor/Actor.h"
#include "Component/SphereCollider.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"

#include "GravityBody.h"

Attractor::Attractor(Actor* actor)
	: Component(actor)
	, mRadius(1.0f)
	, mCenter(Vector3::kZero)
	, mGravity(-9.8f)
	, mIsInvSquare(true)
	, mMass(1.0f)
{

}

Attractor::~Attractor()
{

}

// アクターを引っ張る
void Attractor::Attract(GravityBody* actor, float deltaTime)
{
	Actor* gravityActor = actor->GetOwner();
	Vector3 apos = mOwner->mTransform->GetWorld().GetTranslation();
	Vector3 bpos = gravityActor->mTransform->mPosition;
	Vector3 toCenter = apos - bpos;
	float len = Length(toCenter);
	// 万有引力
	float gravity = 0.0f;
	if (mIsInvSquare)
	{
		gravity = (mGravity * mMass * actor->GetMass() / len * len) * deltaTime;
	}
	else
	{
		gravity = (mGravity * mMass * actor->GetMass() / len) * deltaTime;
	}
	actor->AddForce(gravity);
}

// ==================================================
// json
// ==================================================

void Attractor::Load(const nlohmann::json& json)
{
	JsonHelper::GetFloat(json, "Radius", mRadius);
	JsonHelper::GetFloat(json, "Gravity", mGravity);
	JsonHelper::GetBool(json, "Is Inv Square", mIsInvSquare);
	JsonHelper::GetFloat(json, "Mass", mMass);
}

void Attractor::Save(nlohmann::json& json)
{
	JsonHelper::SetFloat(json, "Radius", mRadius);
	JsonHelper::SetFloat(json, "Gravity", mGravity);
	JsonHelper::SetBool(json, "Is Inv Square", mIsInvSquare);
	JsonHelper::SetFloat(json, "Mass", mMass);
}

// ==================================================
// 開発用
// ==================================================

void Attractor::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Attractor"))
	{
		ImGui::DragFloat("Radius", &mRadius, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Gravity", &mGravity, 0.01f, -100.0f, -0.0f);
		ImGui::Checkbox("Is Inv Square", &mIsInvSquare);
		ImGui::DragFloat("Mass", &mMass, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
}
