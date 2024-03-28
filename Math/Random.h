#pragma once
#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include <random>

class Random
{
public:
	static void Initialize();

	static int Rand(int min, int max);
	static float Rand(float min, float max);
	static Vector2 Rand(const Vector2& min, const Vector2& max);
	static Vector3 Rand(const Vector3& min, const Vector3& max);
	static Color Rand(const Color& min, const Color& max);

private:
	// メルセンヌ・ツイスタ
	static std::mt19937 sEngine;
};
