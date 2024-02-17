#include "BoxCollider.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Helper/JsonHelper.h"
#include "SphereCollider.h"

BoxCollider::BoxCollider(Actor* owner)
	: Collider(owner)
	// Center, Size
	, mLocal(Vector3::kZero, Vector3::kZero)
	, mWorld(Vector3::kZero, Vector3::kZero)
{

}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::OnUpdateWorld()
{
	mWorld = mLocal;
	// ワールドボックスを計算
	auto world = mOwner->mTransform->GetWorld();
	mWorld.mCenter *= world;
	Quaternion rotation = world.GetRotation();
	mWorld.mAxis[0] *= rotation;
	mWorld.mAxis[1] *= rotation;
	mWorld.mAxis[2] *= rotation;
	mWorld.mSize *= world.GetScale();
}

bool BoxCollider::Dispatch(Collider* other, CollisionInfo& info)
{
	return other->IsCollision(this, info);
}

// 球とボックス
bool BoxCollider::IsCollision(SphereCollider* other, CollisionInfo& info)
{
	return Intersect(other->GetSphere(), mWorld, info);
}

// ボックスとボックス
bool BoxCollider::IsCollision(BoxCollider* other, CollisionInfo& info)
{
	if (Intersect(mWorld, other->mWorld, info))
	{
		info.mNormal = -info.mNormal;
		return true;
	}
	return false;
}

// ==================================================
// json
// ==================================================

void BoxCollider::Load(const nlohmann::json& json)
{
	Collider::Load(json);
	JsonHelper::GetVector3(json, "Center", mLocal.mCenter);
	JsonHelper::GetVector3(json, "Axis X", mLocal.mAxis[0]);
	JsonHelper::GetVector3(json, "Axis Y", mLocal.mAxis[1]);
	JsonHelper::GetVector3(json, "Axis Z", mLocal.mAxis[2]);
	JsonHelper::GetVector3(json, "Size", mLocal.mSize);
}

void BoxCollider::Save(nlohmann::json& json)
{
	Collider::Save(json);
	JsonHelper::SetVector3(json, "Center", mLocal.mCenter);
	JsonHelper::SetVector3(json, "Axis X", mLocal.mAxis[0]);
	JsonHelper::SetVector3(json, "Axis Y", mLocal.mAxis[1]);
	JsonHelper::SetVector3(json, "Axis Z", mLocal.mAxis[2]);
	JsonHelper::SetVector3(json, "Size", mLocal.mSize);
}

// ==================================================
// 開発用
// ==================================================

void BoxCollider::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Box Collider"))
	{
		Collider::UpdateForDev();
		ImGui::DragFloat3("Center", &mLocal.mCenter.x, 0.01f);
		// 
		ImGui::DragFloat3("Size", &mLocal.mSize.x, 0.01f);
		OnUpdateWorld();
		ImGui::TreePop();
	}
}

void BoxCollider::RenderForDev(Primitive* prim)
{
	prim->DrawBox3(mWorld, Color::kWhite);
}
