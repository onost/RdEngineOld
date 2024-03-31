#include "JsonLoader.h"
#include "Helper/JsonHelper.h"
#include "RdEngine.h"
#include "Scene/SceneManager.h"
#include <fstream>

const std::string JsonLoader::kLevelPath = "Assets/Level/";

// ==================================================
// システムファイル
// ==================================================

bool JsonLoader::Load(RdEngine* engine, const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	if (!file)
	{
		return false;
	}
	nlohmann::json data;
	file >> data;

	// -------------------- ここから --------------------
	if (data.contains("Scenes"))
	{
		auto sceneManager = engine->GetSceneManager();
		sceneManager->Load(data);
	}
	auto renderer = engine->GetRenderer();
	renderer->LoadFile(data["Renderer"]);
	// -------------------- ここまで --------------------

	return true;
}

bool JsonLoader::Save(RdEngine* engine, const std::string& filePath)
{
	std::ofstream file(filePath.c_str());
	if (!file)
	{
		return false;
	}
	nlohmann::json data;

	// -------------------- ここから --------------------
	auto sceneManager = engine->GetSceneManager();
	sceneManager->Save(data);
	auto renderer = engine->GetRenderer();
	renderer->SaveFile(data["Renderer"]);
	// -------------------- ここまで --------------------

	file << data.dump(4) << std::endl;
	return true;
}

// ==================================================
// シーンファイル
// ==================================================

bool JsonLoader::LoadScene(Scene* scene, const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	if (!file)
	{
		return false;
	}
	nlohmann::json data;
	file >> data;

	// -------------------- ここから --------------------
	// Renderer
	if (data.contains("Renderer"))
	{
		LoadRenderer(data["Renderer"], scene->GetRenderer().get());
	}
	// Actor
	if (data.contains("Actor"))
	{
		LoadActors(data["Actor"], scene);
	}
	// -------------------- ここまで --------------------

	return true;
}

bool JsonLoader::SaveScene(Scene* scene, const std::string& fileName)
{
	std::ofstream file(fileName.c_str());
	if (!file)
	{
		return false;
	}
	nlohmann::json data;

	// -------------------- ここから --------------------
	// Renderer
	SaveRenderer(data["Renderer"], scene->GetRenderer().get());
	// Actor
	SaveActors(data["Actor"], scene);
	// -------------------- ここまで --------------------

	file << data.dump(4) << std::endl;
	//file << data << std::endl;
	return true;
}

// ==================================================
// Prefab
// ==================================================

Actor* JsonLoader::LoadPrefab(Scene* scene, const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	if (!file)
	{
		return nullptr;
	}
	nlohmann::json data;
	file >> data;

	// -------------------- ここから --------------------

	uint32_t type = 0;
	JsonHelper::GetUint(data, "Type", type);
	// アクターを作成
	Actor* actor = Actor::kCreateFuncs[type](scene);
	actor->Load(data);

	// 子アクター
	if (data.contains("Children"))
	{
		LoadChildren(data["Children"], actor);
	}
	// コンポーネント
	if (data.contains("Components"))
	{
		LoadComponents(data["Components"], actor);
	}

	// -------------------- ここまで --------------------

	return actor;
}

bool JsonLoader::SavePrefab(Actor* actor, const std::string& filePath)
{
	std::ofstream file(filePath.c_str());
	if (!file)
	{
		return false;
	}
	nlohmann::json data;

	// -------------------- ここから --------------------

	uint32_t type = static_cast<uint32_t>(actor->GetType());
	JsonHelper::SetUint(data, "Type", type);
	// セーブ
	actor->Save(data);
	// 子アクター
	if (actor->GetChildren().size() > 0)
	{
		SaveChildren(data["Children"], actor);
	}
	// コンポーネント
	SaveComponents(data["Components"], actor);

	// -------------------- ここまで --------------------

	file << data.dump(4) << std::endl;
	//file << data << std::endl;
	return true;
}

// ==================================================
// ロード
// ==================================================

// レンダラー
void JsonLoader::LoadRenderer(const nlohmann::json& json, Renderer* renderer)
{
	renderer->GetLightManager()->LoadLevel(json);
}

// アクター
void JsonLoader::LoadActors(const nlohmann::json& json, Scene* scene)
{
	for (size_t i = 0; i < json.size(); ++i)
	{
		const nlohmann::json& data = json[i];
		uint32_t type = 0;
		JsonHelper::GetUint(data, "Type", type);
		if (type >= Actor::kCreateFuncs.size())
		{
			continue;
		}

		// アクターを作成
		Actor* actor = Actor::kCreateFuncs[type](scene);
		actor->Load(data);

		// 子アクター
		if (data.contains("Children"))
		{
			LoadChildren(data["Children"], actor);
		}
		// コンポーネント
		if (data.contains("Components"))
		{
			LoadComponents(data["Components"], actor);
		}
	}
}

// 子アクター
void JsonLoader::LoadChildren(const nlohmann::json& json, Actor* parent)
{
	for (size_t i = 0; i < json.size(); ++i)
	{
		const nlohmann::json& data = json[i];
		uint32_t type = 0;
		JsonHelper::GetUint(data, "Type", type);
		if (type >= Actor::kCreateFuncs.size())
		{
			continue;
		}

		// アクター名
		std::string name;
		JsonHelper::GetString(data, "Name", name);

		Actor* actor = parent->GetChild(name);// 追加済みか
		if (!actor)
		{
			// アクターを作成
			actor = Actor::kCreateFuncs[type](parent->GetScene());
		}
		actor->Load(data);
		actor->SetParent(parent);

		// 子アクター
		if (data.contains("Children"))
		{
			LoadChildren(data["Children"], actor);
		}
		// コンポーネント
		if (data.contains("Components"))
		{
			LoadComponents(data["Components"], actor);
		}
	}
}

// コンポーネント
void JsonLoader::LoadComponents(const nlohmann::json& json, Actor* owner)
{
	for (size_t i = 0; i < json.size(); ++i)
	{
		const nlohmann::json& data = json[i];
		uint32_t type = 0;
		JsonHelper::GetUint(data, "Type", type);
		if (type >= Component::kCreateFuncs.size())
		{
			continue;
		}

		Component* comp =
			owner->GetComponent(static_cast<Component::Type>(type));// 追加済みか
		if (!comp)
		{
			// コンポーネントを作成
			comp = Component::kCreateFuncs[type](owner);
		}
		comp->Load(data);
	}
}

// ==================================================
// セーブ
// ==================================================

// レンダラー
void JsonLoader::SaveRenderer(nlohmann::json& json, Renderer* renderer)
{
	renderer->GetLightManager()->SaveLevel(json);
}

// アクター
void JsonLoader::SaveActors(nlohmann::json& json, Scene* scene)
{
	auto& actors = scene->GetActors();
	for (auto& actor : actors)
	{
		nlohmann::json data;
		uint32_t type = static_cast<uint32_t>(actor->GetType());
		JsonHelper::SetUint(data, "Type", type);
		// セーブ
		actor->Save(data);
		// 子アクター
		if (actor->GetChildren().size() > 0)
		{
			SaveChildren(data["Children"], actor);
		}
		// コンポーネント
		SaveComponents(data["Components"], actor);
		json.push_back(data);
	}
}

// 子アクター
void JsonLoader::SaveChildren(nlohmann::json& json, Actor* parent)
{
	auto& actors = parent->GetChildren();
	for (auto& actor : actors)
	{
		nlohmann::json data;
		uint32_t type = static_cast<uint32_t>(actor->GetType());
		JsonHelper::SetUint(data, "Type", type);
		// セーブ
		actor->Save(data);
		// 子アクター
		if (actor->GetChildren().size() > 0)
		{
			SaveChildren(data["Children"], actor);
		}
		// コンポーネント
		SaveComponents(data["Components"], actor);
		json.push_back(data);
	}
}

// コンポーネント
void JsonLoader::SaveComponents(nlohmann::json& json, Actor* owner)
{
	auto& comps = owner->GetComponents();
	for (auto& comp : comps)
	{
		nlohmann::json data;
		uint32_t type = static_cast<uint32_t>(comp->GetType());
		JsonHelper::SetUint(data, "Type", type);
		// セーブ
		comp->Save(data);
		json.push_back(data);
	}
}
