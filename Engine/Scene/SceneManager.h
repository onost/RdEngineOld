#pragma once
#include "Scene.h"
#include <deque>

class Texture;

// シーン管理
class SceneManager
{
public:
	void Initialize();
	void Terminate();

	void TransNextScene();// 更新前
	void Input(const InputSystem::State& input);
	void Update(float deltaTime);
	void UpdateWorld();
	void TestCollision();
	// シーンを変更
	void ChangeScene(const std::string& sceneName);
	void Reset();

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json);
	void Save(nlohmann::json& json);

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev();
	void RenderForDev(Primitive* prim);

	Scene* GetCurrScene() const { return mCurrScene.get(); }

	void InitActor();

private:
	std::vector<std::string> mSceneNames;
	std::unique_ptr<Scene> mCurrScene;
	std::unique_ptr<Scene> mNextScene;
	// 開発用
	std::string mSceneName;
	Texture* mFileTex;
};
