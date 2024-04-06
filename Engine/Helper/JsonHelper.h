#pragma once
#include "Color.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <nlohmann/json.hpp>

// json用ヘルパー関数群
namespace JsonHelper
{
	// 読み込み
	void GetBool(const nlohmann::json& json, const char* name, bool& out);
	//void GetInt16(const nlohmann::json& json, const char* name, int16_t& out);
	void GetInt32(const nlohmann::json& json, const char* name, int32_t& out);
	//void GetUint16(const nlohmann::json& json, const char* name, uint16_t& out);
	void GetUint32(const nlohmann::json& json, const char* name, uint32_t& out);
	void GetFloat(const nlohmann::json& json, const char* name, float& out);
	void GetString(const nlohmann::json& json, const char* name, std::string& out);
	void GetColor(const nlohmann::json& json, const char* name, Color& out);
	void GetQuaternion(const nlohmann::json& json, const char* name, Quaternion& out);
	void GetVector2(const nlohmann::json& json, const char* name, Vector2& out);
	void GetVector3(const nlohmann::json& json, const char* name, Vector3& out);
	void GetVector4(const nlohmann::json& json, const char* name, Vector4& out);

	// 保存
	void SetBool(nlohmann::json& json, const char* name, bool in);
	//void SetInt16(nlohmann::json& json, const char* name, int16_t in);
	void SetInt32(nlohmann::json& json, const char* name, int32_t in);
	//void SetUint16(nlohmann::json& json, const char* name, uint16_t in);
	void SetUint32(nlohmann::json& json, const char* name, uint32_t in);
	void SetFloat(nlohmann::json& json, const char* name, float in);
	void SetString(nlohmann::json& json, const char* name, std::string in);
	void SetColor(nlohmann::json& json, const char* name, Color in);
	void SetQuaternion(nlohmann::json& json, const char* name, Quaternion in);
	void SetVector2(nlohmann::json& json, const char* name, Vector2 in);
	void SetVector3(nlohmann::json& json, const char* name, Vector3 in);
	void SetVector4(nlohmann::json& json, const char* name, Vector4 in);
}
