#include "SphereCollider.h"
#include "Actor/Actor.h"
#include "BoxCollider.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Helper/JsonHelper.h"

SphereCollider::SphereCollider(Actor* owner)
	: Collider(owner)
	, mLocal(Vector3::kZero, 0.0f)
	, mWorld(Vector3::kZero, 0.0f)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::OnUpdateWorld()
{
	mWorld = mLocal;
	// ワールド球を計算
	auto world = mOwner->mTransform->GetWorld();
	mWorld.mCenter += world.GetTranslation();
	Vector3 scale = world.GetScale();
	// 最大成分
	float max = MyMath::Min(MyMath::Min(scale.x, scale.y), scale.z);
	mWorld.mRadius *= max;
}

bool SphereCollider::Dispatch(Collider* other, CollisionInfo& info)
{
	return other->IsCollision(this, info);
}

// 球と球
bool SphereCollider::IsCollision(SphereCollider* other, CollisionInfo& info)
{
	if (Intersect(mWorld, other->mWorld, info))
	{
		info.mNormal = -info.mNormal;
		return true;
	}
	return false;
}

// ボックスと球
bool SphereCollider::IsCollision(BoxCollider* other, CollisionInfo& info)
{
	if (Intersect(mWorld, other->GetBox(), info))
	{
		info.mNormal = -info.mNormal;
		return true;
	}
	return false;
}

// ==================================================
// json
// ==================================================

void SphereCollider::Load(const nlohmann::json& json)
{
	Collider::Load(json);
	JsonHelper::GetVector3(json, "Center", mLocal.mCenter);
	JsonHelper::GetFloat(json, "Radius", mLocal.mRadius);
}

void SphereCollider::Save(nlohmann::json& json)
{
	Collider::Save(json);
	JsonHelper::SetVector3(json, "Center", mLocal.mCenter);
	JsonHelper::SetFloat(json, "Radius", mLocal.mRadius);
}

// ==================================================
// 開発用
// ==================================================

void SphereCollider::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Sphere Collider"))
	{
		Collider::UpdateForDev();
		ImGui::DragFloat3("Center", &mLocal.mCenter.x, 0.01f);
		ImGui::DragFloat("Radius", &mLocal.mRadius, 0.01f);
		OnUpdateWorld();
		ImGui::TreePop();
	}
}

void SphereCollider::RenderForDev(Primitive* prim)
{
	prim->DrawSphere(mWorld.mCenter, mWorld.mRadius, Color::kWhite);
}
