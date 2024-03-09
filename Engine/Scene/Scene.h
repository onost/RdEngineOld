#pragma once
#include "Actor/Actor.h"
#include <string>
#include <vector>

class CollisionManager;
class Renderer;

class Scene
{
public:
	Scene(const std::string& sceneName);
	virtual ~Scene();

	void Initialize();
	void Terminate();

	void ProcessInput(const Input::State& input);
	void Update(float deltaTime);
	void UpdateWorld();
	void TestCollision();

	// ==================================================
	// json
	// ==================================================
	void Load();
	void Unload();
	void Save();

	// ==================================================
	// アクター
	// ==================================================
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);
	Actor* GetActor(const std::string& name);
	void SortActors();

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev();
	void RenderForDev(Primitive* prim);

	// ==================================================
	// ヘルパー関数
	// ==================================================
	template <class T>
	static Scene* Create()
	{
		return new T();
	}
	Renderer* GetRenderer() const { return mRenderer; }
	CollisionManager* GetCollisionManager() const { return mCollisionManager; }
	const std::vector<Actor*>& GetActors() const { return mActors; }
	Actor* GetActorForDev() const { return mActorForDev; }
	void SetActorForDev(Actor* actor) { mActorForDev = actor; }

protected:
	//std::string& mName;
	std::string mName;
	// レンダラー
	Renderer* mRenderer;
	// 衝突管理
	CollisionManager* mCollisionManager;

	bool mIsUpdating;// 更新中か
	std::vector<Actor*> mActors;
	std::vector<Actor*> mWaitActors;
	// 開発用
	Actor* mActorForDev;
	Actor::Type mActorType;
	std::string mPrefabName;
};
