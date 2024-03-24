#pragma once
#include "Input/InputSystem.h"
#include <cstdint>
#include <nlohmann/json.hpp>

class Actor;
struct CollisionInfo;
class Primitive;

class Component
{
public:
	// タイプ
	enum class Type
	{
		CameraComponent,
		AABBCollider,
		CapsuleCollider,
		BoxCollider,
		SphereCollider,
		SpriteRenderer,
		MeshRenderer,
		SkinnedMeshRenderer,
		ParticleRenderer,
		MeshCollider,
		Transform,
		MeshParticleRenderer,

		Attractor,
		GravityBody,

		Animator

		/*Transform = 0,
		CameraComponent,
		BoxCollider = 10,
		SphereCollider,
		MeshCollider,
		SpriteRenderer = 20,
		MeshRenderer = 30,
		SkinnedMeshRenderer,
		ParticleRenderer = 40,
		MeshParticleRenderer,
		DirectionalLightComponent = 50,
		PointLightComponent,
		SpotLightComponent*/
	};

	Component(Actor* owner, uint32_t order = 100);
	virtual ~Component();

	virtual void ProcessInput(const InputSystem::State&) {}
	virtual void Update(float) {}
	virtual void OnUpdateWorld() {}
	// Collision
	virtual void OnCollisionEnter(Actor*, CollisionInfo*) {}
	virtual void OnCollisionStay(Actor*, CollisionInfo*) {}
	virtual void OnCollisionExit(Actor*, CollisionInfo*) {}
	virtual void OnTriggerEnter(Actor*) {}
	virtual void OnTriggerStay(Actor*) {}
	virtual void OnTriggerExit(Actor*) {}

	// ==================================================
	// json
	// ==================================================
	virtual void Load(const nlohmann::json& json);
	virtual void Save(nlohmann::json& json);

	// ==================================================
	// 開発用
	// ==================================================
	virtual void UpdateForDev();
	virtual void RenderForDev(Primitive*) {}

	// ==================================================
	// ヘルパー関数
	// ==================================================
	template <class T>
	static Component* Create(Actor* actor)
	{
		return new T(actor);
	}
	virtual Type GetType() const = 0;
	Actor* GetOwner() const { return mOwner; }
	uint32_t GetOrder() const { return mOrder; }
	void SetOrder(uint32_t order);

public:
	static const std::vector<std::function<Component* (Actor*)>> kCreateFuncs;
protected:
	Actor* mOwner;// 所有者
	uint32_t mOrder;// 更新順
};
