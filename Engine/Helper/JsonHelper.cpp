#include "JsonHelper.h"

namespace JsonHelper
{
	bool GetBool(const nlohmann::json& json, const char* name, bool& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_boolean())
			{
				out = v.get<bool>();
				return true;
			}
		}
		return false;
	}

	bool GetInt(const nlohmann::json& json, const char* name, int32_t& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_number_integer())
			{
				out = v.get<int32_t>();
				return true;
			}
		}
		return false;
	}

	bool GetUint(const nlohmann::json& json, const char* name, uint32_t& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_number_unsigned())
			{
				out = v.get<uint32_t>();
				return true;
			}
		}
		return false;
	}

	bool GetFloat(const nlohmann::json& json, const char* name, float& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_number_float())
			{
				out = v.get<float>();
				return true;
			}
		}
		return false;
	}

	bool GetString(const nlohmann::json& json, const char* name, std::string& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_string())
			{
				out = v.get<std::string>();
				return true;
			}
		}
		return false;
	}

	bool GetColor(const nlohmann::json& json, const char* name, Color& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_array() && v.size() == 4)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (!v[i].is_number_float())
					{
						return false;
					}
				}
				out.r = v[0].get<float>();
				out.g = v[1].get<float>();
				out.b = v[2].get<float>();
				out.a = v[3].get<float>();
				return true;
			}
		}
		return false;
	}

	bool GetQuaternion(const nlohmann::json& json, const char* name, Quaternion& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_array() && v.size() == 4)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (!v[i].is_number_float())
					{
						return false;
					}
				}
				out.w = v[0].get<float>();
				out.x = v[1].get<float>();
				out.y = v[2].get<float>();
				out.z = v[3].get<float>();
				return true;
			}
		}
		return false;
	}

	bool GetVector2(const nlohmann::json& json, const char* name, Vector2& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_array() && v.size() == 2)
			{
				for (int i = 0; i < 2; ++i)
				{
					if (!v[i].is_number_float())
					{
						return false;
					}
				}
				out.x = v[0].get<float>();
				out.y = v[1].get<float>();
				return true;
			}
		}
		return false;
	}

	bool GetVector3(const nlohmann::json& json, const char* name, Vector3& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_array() && v.size() == 3)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (!v[i].is_number_float())
					{
						return false;
					}
				}
				out.x = v[0].get<float>();
				out.y = v[1].get<float>();
				out.z = v[2].get<float>();
				return true;
			}
		}
		return false;
	}

	bool GetVector4(const nlohmann::json& json, const char* name, Vector4& out)
	{
		auto it = json.find(name);
		if (it != json.end())
		{
			auto v = json.at(name);
			if (v.is_array() && v.size() == 4)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (!v[i].is_number_float())
					{
						return false;
					}
				}
				out.x = v[0].get<float>();
				out.y = v[1].get<float>();
				out.z = v[2].get<float>();
				out.w = v[3].get<float>();
				return true;
			}
		}
		return false;
	}

	int GetInt(const nlohmann::json& data, const char* name)
	{
		return data[name].get<int>();
	}

	void SetBool(nlohmann::json& json, const char* name, bool in)
	{
		nlohmann::json v(in);
		json[name] = v;
	}

	void SetInt(nlohmann::json& json, const char* name, int32_t in)
	{
		nlohmann::json v(in);
		json[name] = v;
	}

	void SetUint(nlohmann::json& json, const char* name, uint32_t in)
	{
		nlohmann::json v(in);
		json[name] = v;
	}

	void SetFloat(nlohmann::json& json, const char* name, float in)
	{
		nlohmann::json v(in);
		json[name] = v;
	}

	void SetString(nlohmann::json& json, const char* name, const std::string& in)
	{
		nlohmann::json v(in);
		json[name] = v;
	}

	void SetColor(nlohmann::json& json, const char* name, const Color& in)
	{
		nlohmann::json v;
		v.push_back(in.r);
		v.push_back(in.g);
		v.push_back(in.b);
		v.push_back(in.a);
		json[name] = v;
	}

	void SetQuaternion(nlohmann::json& json, const char* name, const Quaternion& in)
	{
		nlohmann::json v;
		v.push_back(in.w);
		v.push_back(in.x);
		v.push_back(in.y);
		v.push_back(in.z);
		json[name] = v;
	}

	void SetVector2(nlohmann::json& json, const char* name, const Vector2& in)
	{
		nlohmann::json v;
		v.push_back(in.x);
		v.push_back(in.y);
		json[name] = v;
	}

	void SetVector3(nlohmann::json& json, const char* name, const Vector3& in)
	{
		nlohmann::json v;
		v.push_back(in.x);
		v.push_back(in.y);
		v.push_back(in.z);
		json[name] = v;
	}

	void SetVector4(nlohmann::json& json, const char* name, const Vector4& in)
	{
		nlohmann::json v;
		v.push_back(in.x);
		v.push_back(in.y);
		v.push_back(in.z);
		v.push_back(in.w);
		json[name] = v;
	}
}
