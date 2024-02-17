#pragma once
#include "Color.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <nlohmann/json.hpp>

namespace JsonHelper
{
	bool GetBool(const nlohmann::json& json, const char* name, bool& out);
	bool GetInt(const nlohmann::json& json, const char* name, int32_t& out);
	bool GetUint(const nlohmann::json& json, const char* name, uint32_t& out);
	bool GetFloat(const nlohmann::json& json, const char* name, float& out);
	bool GetString(const nlohmann::json& json, const char* name, std::string& out);
	bool GetColor(const nlohmann::json& json, const char* name, Color& out);
	bool GetQuaternion(const nlohmann::json& json, const char* name, Quaternion& out);
	bool GetVector2(const nlohmann::json& json, const char* name, Vector2& out);
	bool GetVector3(const nlohmann::json& json, const char* name, Vector3& out);
	bool GetVector4(const nlohmann::json& json, const char* name, Vector4& out);

	void SetBool(nlohmann::json& json, const char* name, bool in);
	void SetInt(nlohmann::json& json, const char* name, int32_t in);
	void SetUint(nlohmann::json& json, const char* name, uint32_t in);
	void SetFloat(nlohmann::json& json, const char* name, float in);
	void SetString(nlohmann::json& json, const char* name, const std::string& in);
	void SetColor(nlohmann::json& json, const char* name, const Color& in);
	void SetQuaternion(nlohmann::json& json, const char* name, const Quaternion& in);
	void SetVector2(nlohmann::json& json, const char* name, const Vector2& in);
	void SetVector3(nlohmann::json& json, const char* name, const Vector3& in);
	void SetVector4(nlohmann::json& json, const char* name, const Vector4& in);
}
