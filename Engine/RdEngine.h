#pragma once
#include "Audio/Audio.h"
#include "Collision/CollisionManager.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"
#include "Window.h"
#include <memory>

class RdEngine
{
public:
	enum class State
	{
		kDev,	// 開発
		kGame	// ゲーム
	};
	enum class GameState
	{
		kDev,	// 開発
		kPlay,	// プレイ
		kPause,	// 停止
		kStep	// コマ送り
	};

	// 実行
	void Run();
	// 終了
	void Stop() { mIsRunning = false; }

	void Initialize();
	void Terminate();

	State GetState() const { return mState; }
	GameState GetGameState() const { return mGameState; }
	Window* GetWindow() const { return mWindow.get(); }
	Input* GetInput() const { return mInput.get(); }
	Renderer* GetRenderer() const { return mRenderer.get(); }
	CollisionManager* GetCollisionManager() const { return mCollisionManager.get(); }
	SceneManager* GetSceneManager() const { return mSceneManager.get(); }

private:
	// ゲームループ用
	void ProcessInput();
	void Update();
	void Render();

	// json
	void Load();
	void Save();

	// 開発用
	void UpdateForDev();

public:
	// エンジン名
	static const std::string kName;
	// バージョン
	static const uint32_t kVersion[3];

private:
	bool mIsRunning;
	State mState;
	GameState mGameState;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Input> mInput;
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<CollisionManager> mCollisionManager;
	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<Audio> mAudio;
};

extern std::unique_ptr<RdEngine> gEngine;
