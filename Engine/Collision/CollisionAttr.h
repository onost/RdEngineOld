#pragma once
#include <cstdint>

// 衝突属性
enum class CollisionAttr : uint32_t
{
	None = 0,
	All = 0xFFFFFFFF,

	Terrain = 1 << 0,
	Allies = 1 << 1,
	Enemies = 1 << 2,

	// User
	Planet = 1 << 3
};

inline uint32_t operator&(CollisionAttr a, CollisionAttr b)
{
	return uint32_t(a) & uint32_t(b);
}

inline uint32_t operator|(CollisionAttr a, CollisionAttr b)
{
	return uint32_t(a) | uint32_t(b);
}

inline CollisionAttr operator~(CollisionAttr a)
{
	return CollisionAttr(~uint32_t(a));
}
