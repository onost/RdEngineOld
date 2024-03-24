#pragma once
#include <nlohmann/json.hpp>
#include <string>

class Actor;
class RdEngine;
class Renderer;
class Scene;

class JsonLoader
{
public:
	static const std::string kLevelPath;

	static bool Load(RdEngine* engine, const std::string& filePath);
	static bool Save(RdEngine* engine, const std::string& filePath);

	static bool LoadScene(Scene* scene, const std::string& filePath);
	static bool SaveScene(Scene* scene, const std::string& filePath);

	static Actor* LoadPrefab(Scene* scene, const std::string& filePath);
	static bool SavePrefab(Actor* actor, const std::string& filePath);

private:
	// ロード
	static void LoadRenderer(const nlohmann::json& json, Renderer* renderer);
	static void LoadActors(const nlohmann::json& json, Scene* scene);
	static void LoadChildren(const nlohmann::json& json, Actor* parent);
	static void LoadComponents(const nlohmann::json& json, Actor* owner);

	// セーブ
	static void SaveRenderer(nlohmann::json& json, Renderer* renderer);
	static void SaveActors(nlohmann::json& json, Scene* scene);
	static void SaveChildren(nlohmann::json& json, Actor* child);
	static void SaveComponents(nlohmann::json& json, Actor* owner);
};
