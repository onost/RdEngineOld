#pragma once
#include <cstdint>
#include <type_traits>

enum class CollisionAttribute : uint32_t
{
	kNone = 0,
	kAll = 0xFFFFFFFF,

	kTerrain = 1 << 0,
	kAllies = 1 << 1,
	kEnemies = 1 << 2,

	// ここから
	kPlanet = 1 << 3
	// ここまで
};

#pragma region operator

inline CollisionAttribute operator|(CollisionAttribute a, CollisionAttribute b)
{
	using T = std::underlying_type_t<CollisionAttribute>;
	return static_cast<CollisionAttribute>(static_cast<T>(a) | static_cast<T>(b));
}

inline CollisionAttribute operator&(CollisionAttribute a, CollisionAttribute b)
{
	using T = std::underlying_type_t<CollisionAttribute>;
	return static_cast<CollisionAttribute>(static_cast<T>(a) & static_cast<T>(b));
}

inline CollisionAttribute operator^(CollisionAttribute a, CollisionAttribute b)
{
	using T = std::underlying_type_t<CollisionAttribute>;
	return static_cast<CollisionAttribute>(static_cast<T>(a) ^ static_cast<T>(b));
}

inline CollisionAttribute operator~(CollisionAttribute a)
{
	using T = std::underlying_type_t<CollisionAttribute>;
	return static_cast<CollisionAttribute>(~static_cast<T>(a));
}

inline CollisionAttribute& operator|=(CollisionAttribute& a, CollisionAttribute b)
{
	a = a | b;
	return a;
}

inline CollisionAttribute& operator&=(CollisionAttribute& a, CollisionAttribute b)
{
	a = a & b;
	return a;
}

inline CollisionAttribute& operator^=(CollisionAttribute& a, CollisionAttribute b)
{
	a = a & b;
	return a;
}

#pragma endregion
