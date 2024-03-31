#include "RdEngine.h"
#include "Core/GraphicsEngine.h"
#include "Editor.h"
#include "Loader/JsonLoader.h"
#include "Random.h"

std::unique_ptr<RdEngine> gEngine = nullptr;

// エンジン名
const std::string RdEngine::kName = "RD ENGINE";
// バージョン
const uint32_t RdEngine::kVersion[3] = { 1,0,0 };

RdEngine::RdEngine()
	: mIsRunning(true)
	, mInputSystem(nullptr)
	, mRenderer(nullptr)
	, mAudioSystem(nullptr)
	, mCollisionManager(nullptr)
	, mSceneManager(nullptr)
	, mFilePath("Assets/Data/System.rd")
{

}

void RdEngine::Initialize()
{
	// エンジン名とバージョンをコンソールへ出力
	Console::Log(std::format(
		"{} Ver.{}.{}.{}\n", kName, kVersion[0], kVersion[1], kVersion[2]));

	// COMを初期化
	[[maybe_unused]] HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	MY_ASSERT(SUCCEEDED(hr));

	// インスタンスを作成
	mWindow = std::make_shared<Window>();
	gGraphicsEngine = std::make_shared<GraphicsEngine>();
	mInputSystem = std::make_shared<InputSystem>();
	mRenderer = std::make_shared<Renderer>();
	mAudioSystem = std::make_shared<AudioSystem>();
	mCollisionManager = std::make_shared<CollisionManager>();
	mSceneManager = std::make_shared<SceneManager>();

	// 初期化
	mWindow->Initialize();
	gGraphicsEngine->Initialize(mWindow.get());
	mInputSystem->Initialize(mWindow.get());
	mRenderer->Initialize();
	mAudioSystem->Initialize();
	//mCollisionManager->Initialize();
	mSceneManager->Initialize();
	Random::Initialize();
	Editor::Initialize(mWindow.get());

	// ファイルを読み込む
	LoadFile();

	//mWindow->SetTitle("Set Title");
}

void RdEngine::Terminate()
{
	// ファイルへ保存
	SaveFile();

	Editor::Terminate();

	if (mSceneManager)
	{
		mSceneManager->Terminate();
	}
	/*if (mCollisionManager)
	{
		mCollisionManager->Terminate();
	}*/
	if (mAudioSystem)
	{
		mAudioSystem->Terminate();
	}
	/*if (mRenderer)
	{
		mRenderer->Terminate();
	}*/
	if (mInputSystem)
	{
		mInputSystem->Terminate();
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

// エンジンを実行
void RdEngine::Run()
{
	Initialize();
	// ゲームループ
	while (!mWindow->ProcessMessage())
	{
		if (!mIsRunning) break;
		Input();
		Update();
		Render();
	}
	Terminate();
}

// ==================================================
// ゲームループ用のヘルパー関数
// ==================================================

// 入力
void RdEngine::Input()
{
	// 次のシーンへ
	mSceneManager->TransNextScene();
	// エディタここから
	Editor::PreProcess();

	// 入力
	mInputSystem->Update();
	const auto& inputState = mInputSystem->GetState();
	Editor::Input(inputState);
	if (Editor::IsUpdate())
	{
		mSceneManager->Input(inputState);
	}
}

// 更新
void RdEngine::Update()
{
	const float kDeltaTime = 1.0f / 60.0f;

	// エディタ
	if (Editor::IsEditor())
	{
		Editor::ShowEditor(this);
	}

	// 更新
	Editor::Update(kDeltaTime);
	if (Editor::IsUpdate())
	{
		mSceneManager->Update(kDeltaTime);
	}
	else
	{
		// ワールド行列だけ更新
		mSceneManager->UpdateWorld();
	}
}

// 描画
void RdEngine::Render()
{
	gGraphicsEngine->SetSrvHeap();
	auto cmdList = gGraphicsEngine->GetCmdList();
	mRenderer->Render(cmdList);

	// エディタここまで
	Editor::PostProcess();

	gGraphicsEngine->PreRender();
	mRenderer->RenderFinal(cmdList);
	gGraphicsEngine->PostRender();
}

// ==================================================
// エンジン用ファイル
// ==================================================

// ファイルを読み込む
void RdEngine::LoadFile()
{
	if (JsonLoader::Load(this, mFilePath))
	{
		Console::Log(std::format("Success: Load \"{}\"\n", mFilePath));
	}
	else
	{
		Console::Log(std::format("Failure: Load \"{}\"\n", mFilePath));
	}
}

// ファイルへ保存
void RdEngine::SaveFile()
{
	if (JsonLoader::Save(this, mFilePath))
	{
		Console::Log(std::format("Success: Save \"{}\"\n", mFilePath));
	}
	else
	{
		Console::Log(std::format("Failure: Save \"{}\"\n", mFilePath));
	}
}
