#include "Collider.h"
#include "Actor/Actor.h"
#include "Collision/CollisionManager.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

Collider::Collider(Actor* owner)
	: Component(owner)
	//, mAttribute(Attribute(0))
	//, mResponse(Attribute(0))
	, mAttribute(CollisionAttr(0))
	, mResponse(CollisionAttr(0))
	, mIsTrigger(false)
{
	mOwner->GetScene()->GetCollisionManager()->AddCollider(this);
}

Collider::~Collider()
{
	mOwner->GetScene()->GetCollisionManager()->RemoveCollider(this);
}

// ==================================================
// json
// ==================================================

void Collider::Load(const nlohmann::json& json)
{
	Component::Load(json);
	uint32_t attr;
	if (JsonHelper::GetUint(json, "Attribute", attr))
	{
		//mAttribute = Attribute(attr);
		mAttribute = CollisionAttr(attr);
	}
	uint32_t responseAttr;
	if (JsonHelper::GetUint(json, "Response Attr", responseAttr))
	{
		//mResponse = Attribute(responseAttr);
		mResponse = CollisionAttr(responseAttr);
	}
	JsonHelper::GetBool(json, "Is Trigger", mIsTrigger);
}

void Collider::Save(nlohmann::json& json)
{
	Component::Load(json);
	JsonHelper::SetUint(json, "Attribute", uint32_t(mAttribute));
	JsonHelper::SetUint(json, "Response Attr", uint32_t(mResponse));
	JsonHelper::SetBool(json, "Is Trigger", mIsTrigger);
}

// ==================================================
// 開発用
// ==================================================

void Collider::UpdateForDev()
{
	//auto attrCount = magic_enum::enum_count<Attribute>();
	auto attrCount = magic_enum::enum_count<CollisionAttr>();
	if (ImGui::TreeNode("Attribute"))
	{
		for (uint32_t i = 0; i < attrCount; ++i)
		{
			/*Attribute attr = magic_enum::enum_value<Attribute>(i);
			bool isCheck = ((mAttribute & attr) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<Attribute>(attr).data(), &isCheck))
			{
				if (isCheck) AddAttribute(attr);
				else RemoveAttribute(attr);
			}*/
			CollisionAttr attr = magic_enum::enum_value<CollisionAttr>(i);
			bool isCheck = ((uint32_t(mAttribute) & uint32_t(attr)) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<CollisionAttr>(attr).data(), &isCheck))
			{
				if (isCheck) AddAttribute(attr);
				else RemoveAttribute(attr);
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Response"))
	{
		for (uint32_t i = 0; i < attrCount; ++i)
		{
			/*Attribute attr = magic_enum::enum_value<Attribute>(i);
			bool isCheck = ((mResponse & attr) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<Attribute>(attr).data(), &isCheck))
			{
				if (isCheck) AddResponse(attr);
				else RemoveResponse(attr);
			}*/

			CollisionAttr attr = magic_enum::enum_value<CollisionAttr>(i);
			bool isCheck = ((uint32_t(mAttribute) & uint32_t(attr)) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<CollisionAttr>(attr).data(), &isCheck))
			{
				if (isCheck) AddResponse(attr);
				else RemoveResponse(attr);
			}
		}
		ImGui::TreePop();
	}

	ImGui::Checkbox("Is Trigger", &mIsTrigger);
}
