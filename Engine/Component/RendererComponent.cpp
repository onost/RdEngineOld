#include "RendererComponent.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"

RendererComponent::RendererComponent(Actor* owner)
	: Component(owner)
	, mIsVisible(true)
{

}

RendererComponent::~RendererComponent()
{

}

void RendererComponent::Load(const nlohmann::json& json)
{
	Component::Load(json);
	JsonHelper::GetBool(json, "Is Visible", mIsVisible);
}

void RendererComponent::Save(nlohmann::json& json)
{
	Component::Save(json);
	JsonHelper::SetBool(json, "Is Visible", mIsVisible);
}

void RendererComponent::UpdateForDev()
{
	Component::UpdateForDev();
	ImGui::Checkbox("Is Visible", &mIsVisible);
}
