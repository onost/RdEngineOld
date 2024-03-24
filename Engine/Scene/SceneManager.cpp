#include "SceneManager.h"
#include "Editor.h"
#include "RdEngine.h"

void SceneManager::Initialize()
{
	auto renderer = gEngine->GetRenderer();
	mFileTex = renderer->GetTexture("Assets/Texture/File.png");

	// 最初のシーン
	if (!mSceneNames.empty())
	{
		ChangeScene(mSceneNames[0]);
	}
}

void SceneManager::Terminate()
{
	if (mCurrScene)
	{
		mCurrScene->Terminate();
	}
}

void SceneManager::TransNextScene()
{
	// 次のシーンへ
	if (mNextScene)
	{
		if (mCurrScene)
		{
			mCurrScene->Terminate();
		}
		mCurrScene = std::move(mNextScene);
		mCurrScene->Initialize();
	}
}

void SceneManager::Input(const InputSystem::State& input)
{
	if (mCurrScene)
	{
		mCurrScene->ProcessInput(input);
	}
}

void SceneManager::Update(float deltaTime)
{
	if (mCurrScene)
	{
		mCurrScene->Update(deltaTime);

		mCurrScene->UpdateWorld();

		mCurrScene->TestCollision();
	}
}

void SceneManager::UpdateWorld()
{
	if (mCurrScene)
	{
		mCurrScene->UpdateWorld();
	}
}

void SceneManager::TestCollision()
{
	if (mCurrScene)
	{
		mCurrScene->TestCollision();
	}
}

// シーンを変更
void SceneManager::ChangeScene(const std::string& sceneName)
{
	auto it = std::find(mSceneNames.begin(), mSceneNames.end(), sceneName);
	if (it != mSceneNames.end())
	{
		mNextScene = std::make_unique<Scene>(*it);
	}
}

void SceneManager::Reset()
{
	if (mCurrScene)
	{
		mCurrScene->Terminate();
		mCurrScene->Initialize();
	}
}

// ==================================================
// json
// ==================================================

void SceneManager::Load(const nlohmann::json& json)
{
	for (uint32_t i = 0; i < json.size(); ++i)
	{
		if (json[i].is_string())
		{
			mSceneNames.emplace_back(json[i].get<std::string>());
		}
	}
}

void SceneManager::Save(nlohmann::json& json)
{
	for (uint32_t i = 0; i < mSceneNames.size(); ++i)
	{
		json.push_back(mSceneNames[i]);
	}
}

// ==================================================
// 開発用
// ==================================================

void SceneManager::UpdateForDev()
{
	// シーンアセット
	ImGui::Begin("Scene Asset", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::InputText("Name", &mSceneName);
	if (ImGui::Button("Create"))
	{
		/*mSceneNames.push_back(
			std::format("Scene {}", mSceneNames.size()));*/// デフォルト名
		mSceneNames.push_back(mSceneName);
	}
	auto size = ImVec2(82.0f / 2.0f, 111.0f / 2.0f);
	for (uint32_t i = 0; i < mSceneNames.size(); ++i)
	{
		if (i > 0)
		{
			ImGui::SameLine(0.0f, 20.0f);
		}
		// シーンアセット
		ImGui::BeginGroup();
		ImGui::Image((void*)(intptr_t)mFileTex->GetDescHandle().ptr, size);
		auto& sceneName = mSceneNames[i];
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))// ドラッグ
		{
			ImGui::SetDragDropPayload("SCENE_PAYLOAD", &sceneName, sizeof(sceneName));
			ImGui::Text(sceneName.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::Text(sceneName.c_str());
		ImGui::EndGroup();
	}
	ImGui::End();

	if (mCurrScene)
	{
		mCurrScene->UpdateForDev();
	}
}

void SceneManager::RenderForDev(Primitive* prim)
{
	if (mCurrScene)
	{
		mCurrScene->RenderForDev(prim);
	}
}
