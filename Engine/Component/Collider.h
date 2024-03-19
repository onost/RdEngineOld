#pragma once
#include "Collision/Collision.h"
#include "Color.h"
#include "Component.h"
#include "Collision/CollisionAttr.h"

class SphereCollider;
class BoxCollider;
class MeshCollider;

// コライダー
class Collider : public Component
{
	friend class CollisionManager;
public:
	// 属性
	/*enum Attribute : uint32_t
	{
		Terrain = 1 << 0,
		Allies = 1 << 1,
		Enemies = 1 << 2,
		Planet = 1 << 3,

		kAll = 0xFFFFFFFF
	};*/

	Collider(Actor* owner);
	virtual ~Collider();

	// ダブルディスパッチ
	virtual bool Dispatch(Collider* other, CollisionInfo& info) = 0;
	virtual bool IsCollision(SphereCollider* other, CollisionInfo& info) = 0;
	virtual bool IsCollision(BoxCollider* other, CollisionInfo& info) = 0;
	//virtual bool IsCollision(MeshCollider* other, CollisionInfo& info) = 0;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	bool GetIsTrigger() const { return mIsTrigger; }
	void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }
	// Attribute
	/*void SetAttribute(Attribute attr) { mAttribute = attr; }
	void AddAttribute(Attribute attr) { mAttribute = Attribute(uint32_t(mAttribute) | uint32_t(attr)); }
	void RemoveAttribute(Attribute attr) { mAttribute = Attribute(uint32_t(mAttribute) & uint32_t(~attr)); }
	void SetResponse(Attribute attr) { mResponse = attr; }
	void AddResponse(Attribute attr) { mResponse = Attribute(uint32_t(mResponse) | uint32_t(attr)); }
	void RemoveResponse(Attribute attr) { mResponse = Attribute(uint32_t(mResponse) & uint32_t(~attr)); }*/
	//void SetColor(const Color& color) { mColor = color; }

	void SetAttribute(CollisionAttr attr) { mAttribute = attr; }
	void AddAttribute(CollisionAttr attr) { mAttribute = CollisionAttr(uint32_t(mAttribute) | uint32_t(attr)); }
	void RemoveAttribute(CollisionAttr attr) { mAttribute = CollisionAttr(uint32_t(mAttribute) & ~uint32_t(attr)); }
	void SetResponse(CollisionAttr attr) { mResponse = attr; }
	void AddResponse(CollisionAttr attr) { mResponse = CollisionAttr(uint32_t(mResponse) | uint32_t(attr)); }
	void RemoveResponse(CollisionAttr attr) { mResponse = CollisionAttr(uint32_t(mResponse) & ~uint32_t(attr)); }

protected:
	//Attribute mAttribute;
	//Attribute mResponse;
	CollisionAttr mAttribute;
	CollisionAttr mResponse;
	bool mIsTrigger;
protected:
	//Color mColor;// 開発用
};
