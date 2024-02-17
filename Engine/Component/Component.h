#pragma once
#include "Input/Input.h"
#include <cstdint>
#include <nlohmann/json.hpp>

class Actor;
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
		MeshParticleRenderer

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

	virtual void ProcessInput(const Input::State&) {}
	virtual void Update(float) {}
	virtual void OnUpdateWorld() {}

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
