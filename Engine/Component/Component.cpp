#include "Component.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"

#include "CameraComponent.h"
//#include "AABBCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "SpriteRenderer.h"
#include "MeshParticleRenderer.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "ParticleRenderer.h"
#include "MeshCollider.h"
#include "Transform.h"
#include "Game/Attractor.h"
#include "Game/GravityBody.h"
#include "Animator.h"

const std::vector<std::function<Component* (Actor*)>> Component::kCreateFuncs =
{
	&Component::Create<CameraComponent>,
	&Component::Create<BoxCollider>,// TODO: Remove
	&Component::Create<BoxCollider>,// TODO: Remove
	&Component::Create<BoxCollider>,
	&Component::Create<SphereCollider>,
	&Component::Create<SpriteRenderer>,
	&Component::Create<MeshRenderer>,
	&Component::Create<SkinnedMeshRenderer>,
	&Component::Create<ParticleRenderer>,
	&Component::Create<MeshCollider>,
	&Component::Create<Transform>,
	&Component::Create<MeshParticleRenderer>,
	&Component::Create<Attractor>,
	&Component::Create<GravityBody>,
	&Component::Create<Animator>
};

Component::Component(Actor* owner, uint32_t order)
	: mOwner(owner)
	, mOrder(order)
{
	mOwner->AddComponent(this);
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}

void Component::Load(const nlohmann::json& json)
{
	JsonHelper::GetUint(json, "Order", mOrder);
}

void Component::Save(nlohmann::json& json)
{
	JsonHelper::SetUint(json, "Order", mOrder);
}

void Component::UpdateForDev()
{
	if (ImGui::InputScalar("Order", ImGuiDataType_U32, &mOrder))
	{
		mOwner->SortComponent();
	}
}

void Component::SetOrder(uint32_t order)
{
	mOrder = order;
	mOwner->SortComponent();
}
