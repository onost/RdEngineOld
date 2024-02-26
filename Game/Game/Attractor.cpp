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
	, mGravity(9.8f)
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
	Vector3 apos = mOwner->mTransform->mPosition;
	Vector3 bpos = gravityActor->mTransform->mPosition;
	Vector3 toCenter = apos - bpos;
	float len = Length(toCenter);
	float gravity = mGravity / len * len;// 質量は無視
	// 引っ張る
	gravityActor->mTransform->mPosition += Normalize(toCenter) * gravity * deltaTime;

	// 姿勢を制御
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f) * gravityActor->mTransform->mRotation;
	Vector3 normal = -Normalize(toCenter);
	Vector3 axis = Cross(up, normal);
	if (Length(axis) > 0.001f)
	{
		axis.Normalize();
		float theta = acosf(Dot(up, normal));
		gravityActor->mTransform->mRotation *= Quaternion(axis, theta);
		gravityActor->mTransform->mRotation.Normalize();
	}
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
		ImGui::DragFloat("Gravity", &mGravity, 0.01f, 0.0f, 100.0f);
		ImGui::Checkbox("Is Inv Square", &mIsInvSquare);
		ImGui::DragFloat("Mass", &mMass, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
}
