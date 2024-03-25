#pragma once
#include <functional>
#include <memory>
#include <vector>

template <typename T>
class Pool
{
private:
	// アイテム
	struct Item
	{
		T mValue;
		// アクティブか
		bool mIsActive;

		Item()
			: mValue()
			, mIsActive(false)
		{}
	};

public:
	void Initialize(uint32_t capacity)
	{
		mItems.reserve(capacity);
		for (uint32_t i = 0; i < capacity; ++i)
		{
			mItems.emplace_back(std::make_unique<Item>());
		}
	}

	// アイテムを確保
	T* Alloc(std::function<void(uint32_t, T&)> initFunc = nullptr)
	{
		// 非アクティブなアイテムを探す
		for (uint32_t i = 0; i < mItems.size(); ++i)
		{
			if (!mItems[i]->mIsActive)
			{
				mItems[i]->mIsActive = true;
				if (initFunc)
				{
					initFunc(i, mItems[i]->mValue);
				}
				return &mItems[i]->mValue;
			}
		}
		// 空きがない
		return nullptr;
	}

	// アイテムを解放
	void Free(T* value)
	{
		for (auto& item : mItems)
		{
			if (&item->mValue == value)
			{
				item->mIsActive = false;
				break;
			}
		}
	}

	uint32_t GetCapacity() const
	{
		return uint32_t(mItems.size());
	}

	// アクティブなアイテムの数
	uint32_t GetActiveCount() const
	{
		uint32_t count = 0;
		for (const auto& item : mItems)
		{
			if (item->mIsActive)
			{
				++count;
			}
		}
		return count;
	}

	// 空きの数
	uint32_t GetFreeCount() const
	{
		return GetCapacity() - GetActiveCount();
	}

private:
	std::vector<std::unique_ptr<Item>> mItems;
};
