#include "RdEngine.h"
#include "Core/GraphicsEngine.h"
#include "Editor.h"
#include "Loader/LevelLoader.h"
#include "Random.h"

// エンジン名
const std::string RdEngine::kName = "RD ENGINE";
// バージョン
const uint32_t RdEngine::kVersion[] = { 0,7,0 };

// 実行
void RdEngine::Run()
{
	// ==================================================
	// 初期化
	// ==================================================
	Initialize();
	std::string str = "Initialization completed!\n";
	Helper::WriteToConsole(str);
	//Console::Log(str);

	// ==================================================
	// ゲームループ
	// ==================================================
	str = std::format("{} is running...\n", kName);
	Helper::WriteToConsole(str);
	//Console::Log(str);
	// ゲームループ
	while (!mWindow->ProcessMessage() && mIsRunning)
	{
		ProcessInput();
		Update();
		Render();
	}

	// ==================================================
	// 終了処理
	// ==================================================
	Terminate();
	str = "Termination completed!\n";
	Helper::WriteToConsole(str);
	//Console::Log(str);
}

void RdEngine::Initialize()
{
	// エンジン名とバージョンを出力
	std::string str = std::format(
		"{} Ver.{}.{}.{}\n", kName, kVersion[0], kVersion[1], kVersion[2]);
	Helper::WriteToConsole(str);
	Console::Log(str);

	mIsRunning = true;
	mState = State::kDev;
	//mState = State::kGame;
	mGameState = GameState::kDev;

	// インスタンス作成
	mWindow = std::make_unique<Window>();
	gGraphicsEngine = std::make_unique<GraphicsEngine>();
	mInput = std::make_unique<Input>();
	mRenderer = std::make_unique<Renderer>();
	mCollisionManager = std::make_unique<CollisionManager>();
	mSceneManager = std::make_unique<SceneManager>();

	// システムファイル
	Load();

	// 初期化
	[[maybe_unused]] HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	MyAssert(SUCCEEDED(hr));
	mWindow->Initialize();
	gGraphicsEngine->Initialize(mWindow.get());
	mInput->Initialize(mWindow.get());
	mRenderer->Initialize();
	//mCollisionManager->Initialize();
	mSceneManager->Initialize();

	Editor::Initialize(mWindow.get());
	Random::Initialize();
}

void RdEngine::Terminate()
{
	// システムファイル
	Save();
	Editor::Terminate();
	if (mSceneManager)
	{
		mSceneManager->Terminate();
	}
	/*if (mCollisionManager)
	{
		mCollisionManager->Terminate();
	}*/
	/*if (mRenderer)
	{
		mRenderer->Terminate();
	}*/
	if (mInput)
	{
		mInput->Terminate();
	}
	if (gGraphicsEngine)
	{
		gGraphicsEngine->Terminate();
	}
	if (mWindow)
	{
		mWindow->Terminate();
	}
	CoUninitialize();
}

// ==================================================
// 入力
// ==================================================
void RdEngine::ProcessInput()
{
	// 更新前
	mSceneManager->PreUpdate();
	Editor::PreProcess();
	// 入力
	mInput->Update();
	auto state = mInput->GetState();
	if (mState == State::kDev)
	{
		// デバッグカメラ
		if (mRenderer->GetIsDebugCamera())
		{
			mRenderer->GetDebugCamera()->Input(state);
		}

		switch (mGameState)
		{
		case GameState::kPlay:
		case GameState::kStep:
			mSceneManager->ProcessInput(state);
			break;
		}
	}
	else// State::Game
	{
		mSceneManager->ProcessInput(state);
	}
}

// ==================================================
// 更新
// ==================================================
void RdEngine::Update()
{
	const float kDeltaTime = 1.0f / 60.0f;
	if (mState == State::kDev)
	{
		UpdateForDev();
		mRenderer->UpdateForDev();
		mSceneManager->UpdateForDev();
		// デバッグカメラ
		if (mRenderer->GetIsDebugCamera())
		{
			mRenderer->GetDebugCamera()->Update(kDeltaTime);
		}

		switch (mGameState)
		{
		case GameState::kPlay:
		case GameState::kStep:
			mSceneManager->Update(kDeltaTime);
			break;
		}
		mSceneManager->UpdateWorld();
		mSceneManager->TestCollision();
	}
	else// State::Game
	{
		mSceneManager->Update(kDeltaTime);
		mSceneManager->UpdateWorld();
		mSceneManager->TestCollision();
	}
}

// ==================================================
// 描画
// ==================================================
void RdEngine::Render()
{
	gGraphicsEngine->SetSrvHeap();

	// メインレンダーターゲットへ描画
	auto cmdList = gGraphicsEngine->GetCmdList();
	mRenderer->PreRendering(cmdList);
	mRenderer->DrawScene(cmdList);
	// デバッグ用描画
	if (mState == State::kDev)
	{
		Primitive* prim = mRenderer->GetPrimitive();
		prim->PreRendering(cmdList);
		mRenderer->RenderForDev();
		mSceneManager->RenderForDev(prim);
		prim->PostRendering();
	}
	mRenderer->PostRendering(cmdList);

	if (mState == State::kDev)
	{
		Console::ShowConsole();
	}
	// エディタ終了
	Editor::PostProcess();

	// 最終レンダーターゲットへ描画
	gGraphicsEngine->PreRendering();
	mRenderer->DrawFinalSprite(cmdList);
	gGraphicsEngine->PostRendering();
}

// ==================================================
// json
// ==================================================

void RdEngine::Load()
{
	// システムファイルを読み込む
	std::string filePath = "Assets/Level/System.rdlv";
	if (LevelLoader::LoadSystem(this, filePath))
	{
		Helper::WriteToConsole(
			std::format("Succeeded in loading: \"{}\"\n", filePath));
	}
	else
	{
		Helper::WriteToConsole(
			std::format("Failed to loading: \"{}\"\n", filePath));
	}
}

void RdEngine::Save()
{
	// システムファイルを保存
	std::string filePath = "Assets/Level/System.rdlv";
	if (LevelLoader::SaveSystem(this, filePath))
	{
		Helper::WriteToConsole(
			std::format("Succeeded in loading: \"{}\"\n", filePath));
	}
	else
	{
		Helper::WriteToConsole(
			std::format("Failed to loading: \"{}\"\n", filePath));
	}
}

// ==================================================
// 開発用
// ==================================================

void RdEngine::UpdateForDev()
{
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoMove);
	if (mGameState == GameState::kStep)
	{
		mGameState = GameState::kPause;
	}
	switch (mGameState)
	{
	case GameState::kDev:
		if (ImGui::Button("Game"))
		{
			mGameState = GameState::kPlay;
			mRenderer->SetIsDebugCamera(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			mGameState = GameState::kPause;
			mRenderer->SetIsDebugCamera(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			mGameState = GameState::kStep;
			mRenderer->SetIsDebugCamera(false);
		}
		break;
	case GameState::kPlay:
		if (ImGui::Button("Dev"))
		{
			mGameState = GameState::kDev;
			mSceneManager->Reset();
			mRenderer->SetIsDebugCamera(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			mGameState = GameState::kPause;
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			mGameState = GameState::kStep;
		}
		break;
	case GameState::kPause:
		if (ImGui::Button("Dev"))
		{
			mGameState = GameState::kDev;
			mSceneManager->Reset();
			mRenderer->SetIsDebugCamera(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Play"))
		{
			mGameState = GameState::kPlay;
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			mGameState = GameState::kStep;
		}
		break;
	}
	// デバッグカメラ
	bool isDebugCamera = mRenderer->GetIsDebugCamera();
	if (ImGui::Checkbox("Is Debug Camera", &isDebugCamera))
	{
		mRenderer->SetIsDebugCamera(isDebugCamera);
	}
	ImGui::End();
}

std::unique_ptr<RdEngine> gEngine = nullptr;
