#include "Scene.h"
#include "Editor.h"
#include "Loader/JsonLoader.h"
#include "RdEngine.h"
#include <format>

Scene::Scene(const std::string& sceneName)
	: mName(sceneName)
	, mRenderer()
	, mCollisionManager()
	, mIsUpdating(false)
	, mActors()
	, mWaitActors()
	, mActorForDev(nullptr)
	, mActorType(Actor::Type::Actor)
{

}

Scene::~Scene()
{

}

void Scene::Initialize()
{
	mRenderer = gEngine->GetRenderer();
	//MyAssert(mRenderer);
	mCollisionManager = gEngine->GetCollisionManager();
	//MyAssert(mCollisionManager);

	// ロード
	Load();
}

void Scene::Terminate()
{
	// アンロード
	Unload();
}

void Scene::ProcessInput(const InputSystem::State& input)
{
	// アクターの入力処理
	mIsUpdating = true;
	for (uint32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->ProcessInput(input);
	}
	mIsUpdating = false;
}

void Scene::Update(float deltaTime)
{
	// アクターの更新処理
	mIsUpdating = true;
	for (uint32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->Update(deltaTime);
	}
	for (uint32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->LastUpdate(deltaTime);
	}
	mIsUpdating = false;

	// 待ちアクターを移動
	for (auto& actor : mWaitActors)
	{
		AddActor(actor);
	}
	mWaitActors.clear();

	// 死んだアクターを削除
	std::vector<Actor*> deadActors;
	for (auto& actor : mActors)
	{
		if (actor->GetState() == Actor::State::kDead)
		{
			deadActors.emplace_back(actor);
		}
	}
	for (auto& actor : deadActors)
	{
		delete actor;
	}

	for (uint32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->RemoveDeadActor();
	}

	//TestCollision();//
}

void Scene::UpdateWorld()
{
	// ワールド行列を計算
	for (auto& actor : mActors)
	{
		actor->UpdateWorld();
	}
}

void Scene::TestCollision()
{
	// すべてのコライダーをテスト
	mCollisionManager->TestAllCollider();
}

void Scene::AddActor(Actor* actor)
{
	if (Editor::gIsGame && actor)
	{
		actor->Initialize();
	}
	if (mIsUpdating)
	{
		mWaitActors.emplace_back(actor);
	}
	else
	{
		uint32_t order = actor->GetOrder();
		auto it = mActors.begin();
		for (; it != mActors.end(); ++it)
		{
			if (order < (*it)->GetOrder())
			{
				break;
			}
		}
		mActors.insert(it, actor);
	}
}

void Scene::RemoveActor(Actor* actor)
{
	// 開発用アクターか
	if (mActorForDev == actor)
	{
		mActorForDev = nullptr;
	}

	auto it = std::find(mActors.begin(), mActors.end(), actor);
	if (it != mActors.end())
	{
		mActors.erase(it);
	}
	it = std::find(mWaitActors.begin(), mWaitActors.end(), actor);
	if (it != mWaitActors.end())
	{
		mWaitActors.erase(it);
	}
}

// アクター名からアクターを取得
Actor* Scene::GetActor(const std::string& name)
{
	for (auto& actor : mActors)
	{
		if (actor->GetName() == name)
		{
			return actor;
		}
		auto child = actor->GetChild(name);
		if (child)
		{
			return child;
		}
	}
	for (auto& actor : mWaitActors)
	{
		if (actor->GetName() == name)
		{
			return actor;
		}
		auto child = actor->GetChild(name);
		if (child)
		{
			return child;
		}
	}
	return nullptr;
}

// 子アクターをソート
void Scene::SortActors()
{
	std::sort(mActors.begin(), mActors.end(),
		[](Actor* a, Actor* b)
		{
			return a->GetOrder() < b->GetOrder();
		}
	);
}

// ==================================================
// 開発用
// ==================================================

void Scene::UpdateForDev()
{
	// ==================================================
	// シーン
	// ==================================================
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove);
	//ImGui::InputText("Scene", &mName);
	ImGui::Text(std::format("Scene: {}", mName).c_str());
	if (ImGui::BeginDragDropTarget())// ドロップ
	{
		if (auto payload = ImGui::AcceptDragDropPayload("SCENE_PAYLOAD"))
		{
			auto sceneName = (std::string*)(payload->Data);
			gEngine->GetSceneManager()->ChangeScene(*sceneName);
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Load"))
	{
		Load();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		Save();
	}
	ImGui::End();

	// ==================================================
	// アクター
	// ==================================================
	ImGui::Begin("Actor", nullptr, ImGuiWindowFlags_NoMove);
	Editor::Combo("Actor", mActorType);
	if (ImGui::Button("Create"))
	{
		Actor::kCreateFuncs[uint32_t(mActorType)](this);
	}
	ImGui::Separator();
	ImGui::InputText("Prefab Name", &mPrefabName);
	if (ImGui::Button("Load"))
	{
		JsonLoader::LoadPrefab(this, "Assets/Prefab/" + mPrefabName + ".rdpr");
	}
	ImGui::Separator();
	// アクターツリー
	ImGui::Text(std::format("Actor Count: {}", mActors.size()).c_str());
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	// ツリーのルート
	bool isOpen = ImGui::TreeNodeEx(
		(void*)(intptr_t)0,
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanAvailWidth,
		"Root");
	if (ImGui::BeginDragDropTarget())// ドロップ
	{
		if (auto payload = ImGui::AcceptDragDropPayload("ACTOR_PAYLOAD"))
		{
			// 親を解除する
			auto actor = (Actor**)(payload->Data);
			(*actor)->SetParent(nullptr);
		}
		ImGui::EndDragDropTarget();
	}
	if (isOpen)
	{
		// アクターツリー
		Editor::ActorTree(mActors, mActorForDev);
		ImGui::TreePop();
	}
	ImGui::End();

	// ==================================================
	// コンポーネント
	// ==================================================
	ImGui::Begin("Component", nullptr, ImGuiWindowFlags_NoMove);
	if (mActorForDev)
	{
		mActorForDev->UpdateForDev();
		ImGui::Separator();
		if (ImGui::Button("Remove##1"))
		{
			delete mActorForDev;
		}
		if (ImGui::Button("Save Prefab"))
		{
			JsonLoader::SavePrefab(mActorForDev, "Assets/Prefab/" + mActorForDev->GetName() + ".rdpr");
		}
	}
	ImGui::End();
}

void Scene::RenderForDev(Primitive* prim)
{
	if (mActorForDev)
	{
		mActorForDev->RenderForDev(prim);
	}
}

void Scene::InitActor()
{
	auto v = mActors;
	for (auto& actor : v)
	{
		actor->Initialize();
	}
}

// ==================================================
// json
// ==================================================

void Scene::Load()
{
	Unload();

	// シーンファイルを読み込む
	std::string filePath = std::format("Assets/Level/{}.rdlv", mName);
	if (JsonLoader::LoadScene(this, filePath))
	{
		//Helper::WriteToConsole(
		//	std::format("Succeeded in loading: \"{}\"\n", filePath));
	}
	else
	{
		//Helper::WriteToConsole(
		//	std::format("Failed to loading: \"{}\"\n", filePath));
	}

	// アクターをソート
	SortActors();
	// 開発用アクター
	if (mActors.size() > 0)
	{
		mActorForDev = mActors[0];
	}
}

void Scene::Unload()
{
	// アンロード
	while (!mWaitActors.empty())
	{
		delete mWaitActors.back();
	}
	while (!mActors.empty())
	{
		delete mActors.back();
	}
}

void Scene::Save()
{
	// シーンファイルを保存
	std::string filePath = std::format("Assets/Level/{}.rdlv", mName);
	if (JsonLoader::SaveScene(this, filePath))
	{
		//Helper::WriteToConsole(
		//	std::format("Succeeded in saving: \"{}\"\n", filePath));
	}
	else
	{
		//Helper::WriteToConsole(
		//	std::format("Failed to saving: \"{}\"\n", filePath));
	}
}
