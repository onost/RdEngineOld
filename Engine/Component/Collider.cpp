#include "Collider.h"
#include "Actor/Actor.h"
#include "Collision/CollisionManager.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

Collider::Collider(Actor* owner)
	: Component(owner)
	, mAttr(Attribute(0))
	, mResponseAttr(Attribute(0))
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
		mAttr = Attribute(attr);
	}
	uint32_t responseAttr;
	if (JsonHelper::GetUint(json, "Response Attr", responseAttr))
	{
		mResponseAttr = Attribute(responseAttr);
	}
}

void Collider::Save(nlohmann::json& json)
{
	Component::Load(json);
	JsonHelper::SetUint(json, "Attribute", uint32_t(mAttr));
	JsonHelper::SetUint(json, "Response Attr", uint32_t(mResponseAttr));
}

// ==================================================
// 開発用
// ==================================================

void Collider::UpdateForDev()
{
	auto attrCount = magic_enum::enum_count<Attribute>();
	if (ImGui::TreeNode("Attribute"))
	{
		for (uint32_t i = 0; i < attrCount; ++i)
		{
			Attribute attr = magic_enum::enum_value<Attribute>(i);
			bool isCheck = ((mAttr & attr) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<Attribute>(attr).data(), &isCheck))
			{
				if (isCheck) AddAttr(attr);
				else RemoveAttr(attr);
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Response"))
	{
		for (uint32_t i = 0; i < attrCount; ++i)
		{
			Attribute attr = magic_enum::enum_value<Attribute>(i);
			bool isCheck = ((mResponseAttr & attr) != 0);
			if (ImGui::Checkbox(magic_enum::enum_name<Attribute>(attr).data(), &isCheck))
			{
				if (isCheck) AddResponseAttr(attr);
				else RemoveResponseAttr(attr);
			}
		}
		ImGui::TreePop();
	}
}
