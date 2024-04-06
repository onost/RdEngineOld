#include "JsonHelper.h"

namespace JsonHelper
{
	void GetBool(const nlohmann::json& json, const char* name, bool& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_boolean())
			{
				out = item.get<bool>();
				return;
			}
		}
		out = false;
	}

	/*
	void GetInt16(const nlohmann::json& json, const char* name, int16_t& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_number_integer())
			{
				out = item.get<int16_t>();
				return;
			}
		}
		out = 0;
	}
	*/

	void GetInt32(const nlohmann::json& json, const char* name, int32_t& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_number_integer())
			{
				out = item.get<int32_t>();
				return;
			}
		}
		out = 0;
	}

	/*
	void GetUint16(const nlohmann::json& json, const char* name, uint16_t& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_number_unsigned())
			{
				out = item.get<uint16_t>();
				return;
			}
		}
		out = 0;
	}
	*/

	void GetUint32(const nlohmann::json& json, const char* name, uint32_t& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_number_unsigned())
			{
				out = item.get<uint32_t>();
				return;
			}
		}
		out = 0;
	}

	void GetFloat(const nlohmann::json& json, const char* name, float& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_number_float())
			{
				out = item.get<float>();
				return;
			}
		}
		out = 0.0f;
	}

	void GetString(const nlohmann::json& json, const char* name, std::string& out)
	{
		if (json.find(name) != json.end())
		{
			auto item = json.at(name);
			if (item.is_string())
			{
				out = item.get<std::string>();
				return;
			}
		}
		out = std::string();
	}

	void GetColor(const nlohmann::json& json, const char* name, Color& out)
	{
		if (json.find(name) != json.end())
		{
			auto items = json.at(name);
			if (items.is_array() && items.size() == 4)
			{
				for (auto& item : items)
				{
					if (!item.is_number_float())
					{
						out = Color();
						return;
					}
				}
				out = Color(
					items[0].get<float>(),
					items[1].get<float>(),
					items[2].get<float>(),
					items[3].get<float>());
				return;
			}
		}
		out = Color();
	}

	void GetQuaternion(const nlohmann::json& json, const char* name, Quaternion& out)
	{
		if (json.find(name) != json.end())
		{
			auto items = json.at(name);
			if (items.is_array() && items.size() == 4)
			{
				for (auto& item : items)
				{
					if (!item.is_number_float())
					{
						out = Quaternion();
						return;
					}
				}
				out = Quaternion(
					items[0].get<float>(),
					items[1].get<float>(),
					items[2].get<float>(),
					items[3].get<float>());
				return;
			}
		}
		out = Quaternion();
	}

	void GetVector2(const nlohmann::json& json, const char* name, Vector2& out)
	{
		if (json.find(name) != json.end())
		{
			auto items = json.at(name);
			if (items.is_array() && items.size() == 2)
			{
				for (auto& item : items)
				{
					if (!item.is_number_float())
					{
						out = Vector2();
						return;
					}
				}
				out = Vector2(
					items[0].get<float>(),
					items[1].get<float>());
				return;
			}
		}
		out = Vector2();
	}

	void GetVector3(const nlohmann::json& json, const char* name, Vector3& out)
	{
		if (json.find(name) != json.end())
		{
			auto items = json.at(name);
			if (items.is_array() && items.size() == 3)
			{
				for (auto& item : items)
				{
					if (!item.is_number_float())
					{
						out = Vector3();
						return;
					}
				}
				out = Vector3(
					items[0].get<float>(),
					items[1].get<float>(),
					items[2].get<float>());
				return;
			}
		}
		out = Vector3();
	}

	void GetVector4(const nlohmann::json& json, const char* name, Vector4& out)
	{
		if (json.find(name) != json.end())
		{
			auto items = json.at(name);
			if (items.is_array() && items.size() == 4)
			{
				for (auto& item : items)
				{
					if (!item.is_number_float())
					{
						out = Vector4();
						return;
					}
				}
				out = Vector4(
					items[0].get<float>(),
					items[1].get<float>(),
					items[2].get<float>(),
					items[3].get<float>());
				return;
			}
		}
		out = Vector4();
	}

	void SetBool(nlohmann::json& json, const char* name, bool in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}

	/*
	void SetInt16(nlohmann::json& json, const char* name, int16_t in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}
	*/

	void SetInt32(nlohmann::json& json, const char* name, int32_t in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}

	/*
	void SetUint16(nlohmann::json& json, const char* name, uint16_t in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}
	*/

	void SetUint32(nlohmann::json& json, const char* name, uint32_t in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}

	void SetFloat(nlohmann::json& json, const char* name, float in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}

	void SetString(nlohmann::json& json, const char* name, std::string in)
	{
		nlohmann::json item(in);
		json[name] = item;
	}

	void SetColor(nlohmann::json& json, const char* name, Color in)
	{
		nlohmann::json items;
		items.push_back(in.r);
		items.push_back(in.g);
		items.push_back(in.b);
		items.push_back(in.a);
		json[name] = items;
	}

	void SetQuaternion(nlohmann::json& json, const char* name, Quaternion in)
	{
		nlohmann::json items;
		items.push_back(in.w);
		items.push_back(in.x);
		items.push_back(in.y);
		items.push_back(in.z);
		json[name] = items;
	}

	void SetVector2(nlohmann::json& json, const char* name, Vector2 in)
	{
		nlohmann::json items;
		items.push_back(in.x);
		items.push_back(in.y);
		json[name] = items;
	}

	void SetVector3(nlohmann::json& json, const char* name, Vector3 in)
	{
		nlohmann::json items;
		items.push_back(in.x);
		items.push_back(in.y);
		items.push_back(in.z);
		json[name] = items;
	}

	void SetVector4(nlohmann::json& json, const char* name, Vector4 in)
	{
		nlohmann::json items;
		items.push_back(in.x);
		items.push_back(in.y);
		items.push_back(in.z);
		items.push_back(in.w);
		json[name] = items;
	}
}
