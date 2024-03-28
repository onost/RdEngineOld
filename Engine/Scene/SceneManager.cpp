#include "SceneManager.h"
#include "Editor.h"
#include "RdEngine.h"
#include "Helper/JsonHelper.h"

void SceneManager::Initialize()
{
	auto renderer = gEngine->GetRenderer();
	mFileTex = renderer->GetTexture("Assets/Texture/File.png");

	// 最初のシーン
	/*if (!mSceneNames.empty())
	{
		ChangeScene(mSceneNames[0]);
	}*/
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
	auto scene = json["Scenes"];
	for (uint32_t i = 0; i < scene.size(); ++i)
	{
		if (scene[i].is_string())
		{
			mSceneNames.emplace_back(scene[i].get<std::string>());
		}
	}

	std::string name;
	JsonHelper::GetString(json, "Curr Scene", name);
	ChangeScene(name);
}

void SceneManager::Save(nlohmann::json& json)
{
	auto& scene = json["Scenes"];
	for (uint32_t i = 0; i < mSceneNames.size(); ++i)
	{
		scene.push_back(mSceneNames[i]);
	}

	json["Curr Scene"] = mCurrScene->GetName();
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
		ImGui::Image((void*)(intptr_t)mFileTex->GetDescHandle()->mGpuHandle.ptr, size);
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

void SceneManager::InitActor()
{
	if (mCurrScene)
	{
		mCurrScene->InitActor();
	}
}
