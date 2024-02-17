#pragma once
#include <memory>
#include <string>
#include <unordered_map>

template <class TResource>
class ResourceBank
{
public:
	void Add(const std::string& filePath, TResource* resource)
	{
		auto it = mResources.find(filePath);
		if (it == mResources.end())
		{
			mResources.emplace(filePath, resource);
		}
	}

	TResource* Get(const std::string& filePath)
	{
		auto it = mResources.find(filePath);
		if (it != mResources.end())
		{
			return it->second.get();
		}
		return nullptr;
	}

	// ゲッター
	const std::unordered_map<std::string, std::unique_ptr<TResource>>&
		GetResources() const { return mResources; }

private:
	std::unordered_map<std::string, std::unique_ptr<TResource>> mResources;
};
