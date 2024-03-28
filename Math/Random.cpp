#include "Random.h"

std::mt19937 Random::sEngine;

void Random::Initialize()
{
	std::random_device rd;
	sEngine.seed(rd());
}

int Random::Rand(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(sEngine);
}

float Random::Rand(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(sEngine);
}

Vector2 Random::Rand(const Vector2& min, const Vector2& max)
{
	Vector2 result = Vector2(
		min.x + (max.x - min.x) * Rand(0.0f, 1.0f),
		min.y + (max.y - min.y) * Rand(0.0f, 1.0f));
	return result;
}

Vector3 Random::Rand(const Vector3& min, const Vector3& max)
{
	Vector3 result = Vector3(
		min.x + (max.x - min.x) * Rand(0.0f, 1.0f),
		min.y + (max.y - min.y) * Rand(0.0f, 1.0f),
		min.z + (max.z - min.z) * Rand(0.0f, 1.0f));
	return result;
}

Color Random::Rand(const Color& min, const Color& max)
{
	Color result = Color(
		min.r + (max.r - min.r) * Rand(0.0f, 1.0f),
		min.g + (max.g - min.g) * Rand(0.0f, 1.0f),
		min.b + (max.b - min.b) * Rand(0.0f, 1.0f),
		min.a + (max.a - min.a) * Rand(0.0f, 1.0f));
	return result;
}
