#pragma once
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Input/InputSystem.h"
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct CollisionInfo;
class Primitive;
class Scene;

class Actor
{
public:
	// 状態
	enum class State
	{
		kAlive,
		kPause,
		kDead
	};
	// タイプ
	enum class Type
	{
		Actor,
		//
		Tanuki01,//
		Tanuki02,//
		DemoPlayer,
		DemoCamera,
		DemoEnemy,
		Player,
		PlayerCamera,
		StarPiece,
		StarPieceManager,
		Meteorite,
		MeteoriteEmitter
		//Attractor
		//
	};

	Actor(Scene* scene, uint32_t order = 100);
	virtual ~Actor();

	// 基本
	virtual void Initialize() {}
	void ProcessInput(const InputSystem::State& input);
	void Update(float deltaTime);
	void LastUpdate(float deltaTime);
	void UpdateWorld(Actor* parent = nullptr);
	// Collision
	void OnCollisionEnter(Actor* other, CollisionInfo* info);
	void OnCollisionStay(Actor* other, CollisionInfo* info);
	void OnCollisionExit(Actor* other, CollisionInfo* info);
	void OnTriggerEnter(Actor* other);
	void OnTriggerStay(Actor* other);
	void OnTriggerExit(Actor* other);

	void RemoveDeadActor();

	// ==================================================
	// json
	// ==================================================
	virtual void Load(const nlohmann::json& json);
	virtual void Save(nlohmann::json& json);

	// ==================================================
	// コンポーネント
	// ==================================================
	void AddComponent(Component* comp);
	void RemoveComponent(Component* comp);
	void SortComponent();
	Component* GetComponent(Component::Type type)
	{
		for (auto& comp : mComponents)
		{
			if (comp->GetType() == type)
			{
				return comp;
			}
		}
		return nullptr;
	}

	// ==================================================
	// 子アクター
	// ==================================================
	void SetParent(Actor* parent);
	Actor* GetChild(const std::string& name)
	{
		for (auto& actor : mChildren)
		{
			if (actor->GetName() == name)
			{
				return actor;
			}
			auto child = actor->GetChild(name);
			if (child)
			{
				return child;
			}
		}
		return nullptr;
	}

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev();
	void RenderForDev(Primitive* prim);

	// ==================================================
	// ヘルパー関数
	// ==================================================
	template <class T>
	static Actor* Create(Scene* scene)
	{
		return new T(scene);
	}
	// Getter
	virtual Type GetType() const { return Type::Actor; }
	Scene* GetScene() const { return mScene; }
	const std::string& GetName() const { return mName; }
	uint32_t GetOrder() const { return mOrder; }
	State GetState() const { return mState; }
	const std::vector<Component*>& GetComponents() const { return mComponents; }
	Actor* GetParent() const { return mParent; }
	const std::vector<Actor*>& GetChildren() const { return mChildren; }
	// Setter
	void SetName(const std::string& name) { mName = name; }
	void SetOrder(uint32_t order);
	void SetState(State state) { mState = state; }

	// ==================================================
	// Instantiate
	// ==================================================
	Actor* Instantiate(const std::string& name);

protected:
	// ==================================================
	// アクター固有
	// ==================================================
	virtual void ActorInput(const InputSystem::State&) {}
	virtual void ActorUpdate(float) {}
	virtual void ActorLastUpdate(float) {}
	// Collision
	virtual void ActorOnCollisionEnter(Actor*, CollisionInfo*) {}
	virtual void ActorOnCollisionStay(Actor*, CollisionInfo*) {}
	virtual void ActorOnCollisionExit(Actor*, CollisionInfo*) {}
	virtual void ActorOnTriggerEnter(Actor*) {}
	virtual void ActorOnTriggerStay(Actor*) {}
	virtual void ActorOnTriggerExit(Actor*) {}
	// 開発用
	virtual void ActorUpdateForDev() {}
	virtual void ActorRenderForDev(Primitive* /*prim*/) {}

private:
	// ==================================================
	// 子アクター
	// ==================================================
	void AddChild(Actor* child);
	void RemoveChild(Actor* child);
	void SortChildren();

public:
	static const std::vector<std::function<Actor* (Scene*)>> kCreateFuncs;
	Transform* mTransform;
protected:
	Scene* mScene;
	std::string mName;
	uint32_t mOrder;
	State mState;
	std::vector<Component*> mComponents;
	Actor* mParent;
	std::vector<Actor*> mChildren;
	// コンポーネントタイプ
	Component::Type mCompType;// 開発用
};

Vector3 GetAllScale(Actor* actor);
