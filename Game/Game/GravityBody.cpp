#include "GravityBody.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"

#include "Attractor.h"

GravityBody::GravityBody(Actor* owner)
	: Component(owner)
	, mAttractor(nullptr)
	, mMass(1.0f)
{

}

GravityBody::~GravityBody()
{

}

void GravityBody::Update(float deltaTime)
{
	if (mAttractor)
	{
		mAttractor->Attract(this, deltaTime);
	}
}

// ==================================================
// json
// ==================================================

void GravityBody::Load(const nlohmann::json& json)
{
	JsonHelper::GetFloat(json, "Mass", mMass);
}

void GravityBody::Save(nlohmann::json& json)
{
	JsonHelper::SetFloat(json, "Mass", mMass);
}

// ==================================================
// 開発用
// ==================================================

void GravityBody::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Gravity Body"))
	{
		ImGui::DragFloat("Mass", &mMass, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
}
