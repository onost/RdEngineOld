#pragma once
#include <map>
#include <memory>
#include <string>
//#include <unordered_map>

template <typename T>
class ResourceBank
{
public:
	ResourceBank()
		: mResources()
	{}

	void Add(const std::string& name, std::shared_ptr<T> resource)
	{
		auto it = mResources.find(name);
		if (it == mResources.end())
		{
			mResources.emplace(name, resource);
		}
	}

	std::shared_ptr<T> Get(const std::string& name)
	{
		auto it = mResources.find(name);
		if (it != mResources.end())
		{
			return it->second;
		}
		return nullptr;
	}

	const std::map<std::string, std::shared_ptr<T>>& GetResources() const { return mResources; }

private:
	std::map<std::string, std::shared_ptr<T>> mResources;
};
