#include "Actor.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"
#include "Loader/JsonLoader.h"

//#include "Demo/Tanuki01.h"
//#include "Demo/Tanuki02.h"
#include "Demo/DemoPlayer.h"
#include "Demo/DemoCamera.h"
#include "Demo/DemoEnemy.h"
#include "Game/Player.h"
#include "Game/Attractor.h"
#include "Game/PlayerCamera.h"
#include "Game/StarPiece.h"
#include "Game/StarPieceManager.h"
#include "Game/Meteorite.h"
#include "Game/MeteoriteEmitter.h"

const std::vector<std::function<Actor* (Scene*)>> Actor::kCreateFuncs =
{
	&Actor::Create<Actor>,
	//
	&Actor::Create<DemoPlayer>,
	&Actor::Create<DemoPlayer>,
	&Actor::Create<DemoPlayer>,
	&Actor::Create<DemoCamera>,
	&Actor::Create<DemoEnemy>,
	&Actor::Create<Player>,
	&Actor::Create<PlayerCamera>,
	&Actor::Create<StarPiece>,
	&Actor::Create<StarPieceManager>,
	&Actor::Create<Meteorite>,
	&Actor::Create<MeteoriteEmitter>,
	//&Actor::Create<Attractor>
	//
};

Actor::Actor(Scene* scene, uint32_t order)
	: mScene(scene)
	, mName("None")
	, mOrder(order)
	, mState(State::kAlive)
	, mComponents()
	, mParent(nullptr)
	, mChildren()
	, mCompType(Component::Type(0))
	, mTransform(nullptr)
{
	mTransform = new Transform(this);
	mScene->AddActor(this);
}

Actor::~Actor()
{
	if (mParent)
	{
		mParent->RemoveChild(this);
	}
	else
	{
		mScene->RemoveActor(this);
	}

	// コンポーネント
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
	// 子アクター
	while (!mChildren.empty())
	{
		delete mChildren.back();
	}
}

void Actor::ProcessInput(const InputSystem::State& input)
{
	if (mState == State::kAlive)
	{
		ActorInput(input);
		for (auto& comp : mComponents)
		{
			comp->ProcessInput(input);
		}
		for (auto& child : mChildren)
		{
			child->ProcessInput(input);
		}
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == State::kAlive)
	{
		ActorUpdate(deltaTime);
		for (auto& comp : mComponents)
		{
			comp->Update(deltaTime);
		}
		for (auto& child : mChildren)
		{
			child->Update(deltaTime);
		}
	}
}

void Actor::LastUpdate(float deltaTime)
{
	if (mState == State::kAlive)
	{
		ActorLastUpdate(deltaTime);
		/*for (auto& comp : mComponents)
		{
			comp->LastUpdate(deltaTime);
		}*/
		for (auto& child : mChildren)
		{
			child->LastUpdate(deltaTime);
		}
	}
}

void Actor::UpdateWorld(Actor* parent)
{
	if (parent)
	{
		mTransform->UpdateWorld(parent->mTransform);
	}
	else
	{
		mTransform->UpdateWorld(nullptr);
	}
	for (auto& comp : mComponents)
	{
		comp->OnUpdateWorld();
	}

	for (auto& child : mChildren)
	{
		child->UpdateWorld(this);
	}
}

// ==================================================
// Collision
// ==================================================

void Actor::OnCollisionEnter(Actor* other, CollisionInfo* info)
{
	if (mState == State::kAlive)
	{
		ActorOnCollisionEnter(other, info);
		for (auto& comp : mComponents)
		{
			comp->OnCollisionEnter(other, info);
		}
	}
}

void Actor::OnCollisionStay(Actor* other, CollisionInfo* info)
{
	if (mState == State::kAlive)
	{
		ActorOnCollisionStay(other, info);
		for (auto& comp : mComponents)
		{
			comp->OnCollisionStay(other, info);
		}
	}
}

void Actor::OnCollisionExit(Actor* other, CollisionInfo* info)
{
	if (mState == State::kAlive)
	{
		ActorOnCollisionExit(other, info);
		for (auto& comp : mComponents)
		{
			comp->OnCollisionExit(other, info);
		}
	}
}

void Actor::OnTriggerEnter(Actor* other)
{
	if (mState == State::kAlive)
	{
		ActorOnTriggerEnter(other);
		for (auto& comp : mComponents)
		{
			comp->OnTriggerEnter(other);
		}
	}
}

void Actor::OnTriggerStay(Actor* other)
{
	if (mState == State::kAlive)
	{
		ActorOnTriggerStay(other);
		for (auto& comp : mComponents)
		{
			comp->OnTriggerStay(other);
		}
	}
}

void Actor::OnTriggerExit(Actor* other)
{
	if (mState == State::kAlive)
	{
		ActorOnTriggerExit(other);
		for (auto& comp : mComponents)
		{
			comp->OnTriggerExit(other);
		}
	}
}

void Actor::RemoveDeadActor()
{
	// 死んだアクターを削除
	std::vector<Actor*> deadActors;
	for (auto& child : mChildren)
	{
		if (child->GetState() == Actor::State::kDead)
		{
			deadActors.emplace_back(child);
		}
	}
	for (auto& actor : deadActors)
	{
		delete actor;
	}
}

// ==================================================
// json
// ==================================================

void Actor::Load(const nlohmann::json& json)
{
	JsonHelper::GetString(json, "Name", mName);
	JsonHelper::GetUint(json, "Order", mOrder);
	uint32_t state = 0;
	JsonHelper::GetUint(json, "State", state);
	mState = State(state);
}

void Actor::Save(nlohmann::json& json)
{
	JsonHelper::SetString(json, "Name", mName);
	JsonHelper::SetUint(json, "Order", mOrder);
	JsonHelper::SetUint(json, "State", uint32_t(mState));
}

// ==================================================
// コンポーネント
// ==================================================

void Actor::AddComponent(Component* comp)
{
	//if (!GetComponent(comp->GetType()))
	{
		mComponents.emplace_back(comp);
	}
}

void Actor::RemoveComponent(Component* comp)
{
	auto it = std::find(mComponents.begin(), mComponents.end(), comp);
	if (it != mComponents.end())
	{
		mComponents.erase(it);
	}
}

// コンポーネントをソート
void Actor::SortComponent()
{
	std::sort(mComponents.begin(), mComponents.end(),
		[](Component* a, Component* b)
		{
			return a->GetOrder() < b->GetOrder();
		}
	);
}

// ==================================================
// 子アクター
// ==================================================

void Actor::SetParent(Actor* parent)
{
	if (mParent == parent)
	{
		return;
	}
	for (auto& child : mChildren)
	{
		if (child == parent)
		{
			return;
		}
	}

	if (mParent)
	{
		auto world = mTransform->GetWorld();
		mTransform->mScale = world.GetScale();
		mTransform->mRotation = world.GetRotation();
		mTransform->mPosition = world.GetTranslation();
	}
	if (parent)
	{
		auto world = mTransform->GetWorld();
		auto local = world * Inverse(parent->mTransform->GetWorld());
		mTransform->mScale = local.GetScale();
		mTransform->mRotation = local.GetRotation();
		mTransform->mPosition = local.GetTranslation();
	}

	if (parent)
	{
		if (mParent)
		{
			mParent->RemoveChild(this);
			parent->AddChild(this);
		}
		else
		{
			mScene->RemoveActor(this);
			parent->AddChild(this);
		}
	}
	else
	{
		if (mParent)
		{
			mParent->RemoveChild(this);
			mScene->AddActor(this);
		}
		/*else
		{

		}*/
	}
	mParent = parent;
}

void Actor::AddChild(Actor* child)
{
	mChildren.emplace_back(child);
}

void Actor::RemoveChild(Actor* child)
{
	// 開発用アクターか
	if (mScene->GetActorForDev() == child)
	{
		mScene->SetActorForDev(nullptr);
	}

	auto it = std::find(mChildren.begin(), mChildren.end(), child);
	if (it != mChildren.end())
	{
		mChildren.erase(it);
	}
}

// 子アクターをソート
void Actor::SortChildren()
{
	std::sort(mChildren.begin(), mChildren.end(),
		[](Actor* a, Actor* b)
		{
			return a->GetOrder() < b->GetOrder();
		}
	);
}

// ==================================================
// 開発用
// ==================================================

void Actor::UpdateForDev()
{
	// アクター名
	ImGui::InputText("Name", &mName);
	if (mName.empty())
	{
		mName = "None";
	}
	// 更新順
	if (ImGui::InputScalar("Order", ImGuiDataType_U32, &mOrder))
	{
		if (mParent)
		{
			mParent->SortChildren();
		}
		else
		{
			mScene->SortActors();
		}
	}
	// アクター固有
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(mName.c_str()))
	{
		ActorUpdateForDev();
		ImGui::TreePop();
	}
	ImGui::Separator();

	// コンポーネント
	for (auto& comp : mComponents)
	{
		comp->UpdateForDev();
		ImGui::Separator();
	}
	Editor::Combo(" ", mCompType);
	ImGui::SameLine();
	if (ImGui::Button("Add"))
	{
		Component::kCreateFuncs[uint32_t(mCompType)](this);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove##0"))
	{
		Component* comp = GetComponent(mCompType);
		if (comp)
		{
			delete comp;
		}
	}
}

void Actor::RenderForDev(Primitive* primitive)
{
	ActorRenderForDev(primitive);
	for (auto& comp : mComponents)
	{
		comp->RenderForDev(primitive);
	}
}

// ==================================================
// ヘルパー関数
// ==================================================

void Actor::SetOrder(uint32_t order)
{
	mOrder = order;
	if (mParent)
	{
		mParent->SortChildren();
	}
	else
	{
		mScene->SortActors();
	}
}

// 親スケールを計算
Vector3 GetAllScale(Actor* actor)
{
	Actor* a = actor;
	Vector3 result = Vector3::kOne;
	while (a->GetParent())
	{
		result *= a->mTransform->mScale;
		a = a->GetParent();
	}
	return result;
}

Actor* Actor::Instantiate(const std::string& name)
{
	auto instance = JsonLoader::LoadPrefab(mScene, "Assets/Prefab/" + name + ".rdpr");
	return instance;
}
