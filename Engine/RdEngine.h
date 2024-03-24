#pragma once
#include "Audio/AudioSystem.h"
#include "Collision/CollisionManager.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Scene/SceneManager.h"
#include "Window.h"
#include <memory>

// RDエンジン
class RdEngine
{
public:
	RdEngine();

	void Initialize();
	void Terminate();
	// エンジンを実行
	void Run();
	// エンジンを終了
	void Stop() { mIsRunning = false; }

	// アクセッサ
	std::shared_ptr<Window> GetWindow() const { return mWindow; }
	std::shared_ptr<InputSystem> GetInputSystem() const { return mInputSystem; }
	std::shared_ptr<Renderer> GetRenderer() const { return mRenderer; }
	std::shared_ptr<AudioSystem> GetAudioSystem() const { return mAudioSystem; }
	std::shared_ptr<CollisionManager> GetCollisionManager() const { return mCollisionManager; }
	std::shared_ptr<SceneManager> GetSceneManager() const { return mSceneManager; }

private:
	// ゲームループ用のヘルパー関数
	void Input();
	void Update();
	void Render();

	// エンジン用ファイル
	void LoadFile();
	void SaveFile();

public:
	// エンジン名
	static const std::string kName;
	// バージョン
	static const uint32_t kVersion[3];

private:
	bool mIsRunning;// 実行中か

	std::shared_ptr<Window> mWindow;
	std::shared_ptr<InputSystem> mInputSystem;
	std::shared_ptr<Renderer> mRenderer;
	std::shared_ptr<AudioSystem> mAudioSystem;
	std::shared_ptr<CollisionManager> mCollisionManager;
	std::shared_ptr<SceneManager> mSceneManager;

	// エンジン用ファイルパス
	std::string mFilePath;
};

// グローバル
extern std::unique_ptr<RdEngine> gEngine;
