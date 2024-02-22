#pragma once
#include "Collision/Collision.h"
#include "Color.h"
#include "Component.h"

class SphereCollider;
class BoxCollider;
class MeshCollider;

// コライダー
class Collider : public Component
{
	friend class CollisionManager;
public:
	// 属性
	enum Attribute : uint32_t
	{
		Terrain = 1 << 0,
		Allies = 1 << 1,
		Enemies = 1 << 2,

		kAll = 0xFFFFFFFF
	};

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
	void SetAttr(Attribute attr) { mAttr = attr; }
	void AddAttr(Attribute attr) { mAttr = Attribute(uint32_t(mAttr) | uint32_t(attr)); }
	void RemoveAttr(Attribute attr) { mAttr = Attribute(uint32_t(mAttr) & uint32_t(~attr)); }
	void SetResponseAttr(Attribute attr) { mResponseAttr = attr; }
	void AddResponseAttr(Attribute attr) { mResponseAttr = Attribute(uint32_t(mResponseAttr) | uint32_t(attr)); }
	void RemoveResponseAttr(Attribute attr) { mResponseAttr = Attribute(uint32_t(mResponseAttr) & uint32_t(~attr)); }
	//void SetColor(const Color& color) { mColor = color; }

protected:
	Attribute mAttr;
	Attribute mResponseAttr;
	bool mIsTrigger;
protected:
	//Color mColor;// 開発用
};
