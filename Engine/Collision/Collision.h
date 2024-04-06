#pragma once
#include "Matrix4.h"
#include "MyMath.h"
#include "Vector3.h"
#include <algorithm>

class Actor;
class Collider;

struct CollisionInfo
{
	Vector3 mNormal;
	float mDepth;
};

struct RaycastInfo
{
	Vector3 mPoint;
	Vector3 mNormal;
	Collider* mCollider;
	Actor* mActor;
};

// 直線
struct Line
{
	Line(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}

	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}

	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = std::numeric_limits<float>::lowest();
	static constexpr float kMaxT = (std::numeric_limits<float>::max)();
};

// 半直線
struct Ray
{
	Ray(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}

	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}

	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = 0.0f;
	static constexpr float kMaxT = (std::numeric_limits<float>::max)();
};

// 線分
struct Segment
{
	Segment(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}

	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}

	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = 0.0f;
	static constexpr float kMaxT = 1.0f;
};

// 三角形
struct Triangle
{
	Triangle()
		: mVertices{ Vector3::kZero,Vector3::kZero,Vector3::kZero }
		, mNormal(Vector3::kZero)
	{}

	Triangle(const Vector3& v1, const Vector3& v2, const Vector3& v3)
		: mVertices{ v1,v2,v3 }
	{
		UpdateNormal();
	}

	void UpdateNormal()
	{
		mNormal = Cross(mVertices[1] - mVertices[0], mVertices[2] - mVertices[0]);
		mNormal.Normalize();
	}

	Vector3 mVertices[3];
	Vector3 mNormal;
};

// 平面
struct Plane
{
	Plane(const Vector3& v, const Vector3& n)
		: mNormal(n)
		, mDistance(0.0f)
	{
		mDistance = Dot(v, mNormal);
	}

	Plane(const Vector3& v1, const Vector3& v2, const Vector3& v3)
		: mNormal(Vector3::kZero)
		, mDistance(0.0f)
	{
		mNormal = Cross(v2 - v1, v3 - v1);
		mNormal.Normalize();
		mDistance = Dot(v1, mNormal);
	}

	Plane(const Triangle& t)
		: mNormal(Vector3::kZero)
		, mDistance(0.0f)
	{
		mNormal = t.mNormal;
		mDistance = Dot(t.mVertices[0], mNormal);
	}

	Vector3 mNormal;
	float mDistance;
};

// 球
struct Sphere
{
	Sphere(const Vector3& center, float radius)
		: mCenter(center)
		, mRadius(radius)
	{}

	Vector3 mCenter;
	float mRadius;
};

// AABB
struct AABB
{
	AABB(const Vector3& min, const Vector3& max)
		: mMin(min)
		, mMax(max)
	{}

	Vector3 mMin;
	Vector3 mMax;
};

// OBB
struct OBB
{
	OBB()
		: mCenter(Vector3::kZero)
		, mAxes{ Vector3(1.0f,0.0f,0.0f),Vector3(0.0f,1.0f,0.0f),Vector3(0.0f,0.0f,1.0f) }
		, mSize(Vector3::kZero)
	{}

	OBB(const Vector3& center, const Vector3& size)
		: mCenter(center)
		, mAxes{ Vector3(1.0f,0.0f,0.0f),Vector3(0.0f,1.0f,0.0f),Vector3(0.0f,0.0f,1.0f) }
		, mSize(size)
	{}

	Matrix4 GetInverse() const
	{
		float tmp[4][4] =
		{
			{ mAxes[0].x,mAxes[1].x,mAxes[2].x,0.0f },
			{ mAxes[0].y,mAxes[1].y,mAxes[2].y,0.0f },
			{ mAxes[0].z,mAxes[1].z,mAxes[2].z,0.0f },
			{ -Dot(mCenter, mAxes[0]),-Dot(mCenter, mAxes[1]),-Dot(mCenter, mAxes[2]),1.0f }
		};
		return Matrix4(tmp);
	}

	Vector3 mCenter;
	Vector3 mAxes[3];
	Vector3 mSize;
};

// カプセル
struct Capsule
{
	Capsule()
		: mSegment(Vector3::kZero, Vector3::kZero)
		, mRadius(0.0f)
	{}

	Capsule(Segment segment, float radius)
		: mSegment(segment)
		, mRadius(radius)
	{}

	Segment mSegment;
	float mRadius;
};

// ==================================================
// Closest
// ==================================================

template <typename T>
Vector3 Closest(const Vector3& _a, const T& _b, float& _t)
{
	Vector3 ab = _b.mEnd - _b.mStart;
	Vector3 ac = _a - _b.mStart;
	_t = Dot(ac, ab) / Dot(ab, ab);
	_t = std::clamp(_t, T::kMinT, T::kMaxT);
	return _b.GetPoint(_t);
}

Vector3 Closest(const Vector3& _a, const AABB& _b);

// ==================================================
// Distance
// ==================================================

template <typename T>
float Distance(const Vector3& _a, const T& _b, float& _t)
{
	Vector3 point = Closest(_a, _b, _t);
	return Length(point - _a);
}

float Distance(const Line& _a, const Line& _b, float& _t1, float& _t2);
float Distance(const Segment& _a, const Segment& _b, float& _t1, float& _t2);
float Distance(const Vector3& _a, const Plane& _b);
float Distance(const Segment& _a, const Plane& _b);

// ==================================================
// Intersect
// ==================================================

template <typename T>
bool Intersect(const T& _a, const Triangle& _b, float& _t, Vector3& _n)
{
	if (Intersect(_a, Plane(_b), _t, _n))
	{
		Vector3 point = _a.GetPoint(_t);
		Vector3 cross1 = Cross(_b.mVertices[1] - _b.mVertices[0], point - _b.mVertices[1]);
		Vector3 cross2 = Cross(_b.mVertices[2] - _b.mVertices[1], point - _b.mVertices[2]);
		Vector3 cross3 = Cross(_b.mVertices[0] - _b.mVertices[2], point - _b.mVertices[0]);
		if (Dot(cross1, _b.mNormal) >= 0.0f &&
			Dot(cross2, _b.mNormal) >= 0.0f &&
			Dot(cross3, _b.mNormal) >= 0.0f)
		{
			if (_t >= T::kMinT && _t <= T::kMaxT)
			{
				return true;
			}
		}
	}
	return false;
}

template <typename T>
bool Intersect(const T& _a, const Plane& _b, float& _t, Vector3& _n)
{
	float dot = Dot(_a.mEnd - _a.mStart, _b.mNormal);
	if (std::abs(dot) < MyMath::kEpsilon)// 平行か？
	{
		return false;
	}
	_t = (_b.mDistance - Dot(_a.mStart, _b.mNormal)) / dot;
	if (_t >= T::kMinT && _t <= T::kMaxT)
	{
		_n = _b.mNormal;
		return true;
	}
	return false;
}

template <typename T>
bool Intersect(const T& _a, const Sphere& _b, float& _t, Vector3& _n)
{
	Vector3 ab = _a.mEnd - _a.mStart;
	Vector3 ac = _a.mStart - _b.mCenter;
	float a = Dot(ab, ab);
	float b = 2.0f * Dot(ac, ab);
	float c = Dot(ac, ac) - _b.mRadius * _b.mRadius;
	float d = b * b - 4.0f * a * c;// b^2-4ac
	if (d < 0.0f)
	{
		return false;
	}
	else
	{
		d = std::sqrt(d);
		float min = (-b - d) / (2.0f * a);
		float max = (-b + d) / (2.0f * a);
		if (min >= T::kMinT && min <= T::kMaxT)
		{
			_t = min;
			_n = Normalize(_a.mStart + min * ab - _b.mCenter);
			return true;
		}
		else if (max >= T::kMinT && max <= T::kMaxT)
		{
			_t = max;
			_n = Normalize(_a.mStart + max * ab - _b.mCenter);
			return true;
		}
		else
		{
			return false;
		}
	}
}

template <typename T>
bool Intersect(const T& _a, const AABB& _b, float& _t, Vector3& _n)
{
	Vector3 ab = _a.mEnd - _a.mStart;
	Vector3 minima = {};
	minima.x = (-_a.mStart.x + _b.mMin.x) / ab.x;
	minima.y = (-_a.mStart.y + _b.mMin.y) / ab.y;
	minima.z = (-_a.mStart.z + _b.mMin.z) / ab.z;
	Vector3 maxima = {};
	maxima.x = (-_a.mStart.x + _b.mMax.x) / ab.x;
	maxima.y = (-_a.mStart.y + _b.mMax.y) / ab.y;
	maxima.z = (-_a.mStart.z + _b.mMax.z) / ab.z;
	Vector3 nears = {};
	nears.x = (std::min)(minima.x, maxima.x);
	nears.y = (std::min)(minima.y, maxima.y);
	nears.z = (std::min)(minima.z, maxima.z);
	Vector3 fars = {};
	fars.x = (std::max)(minima.x, maxima.x);
	fars.y = (std::max)(minima.y, maxima.y);
	fars.z = (std::max)(minima.z, maxima.z);
	float min = (std::max)((std::max)(nears.x, nears.y), nears.z);
	float max = (std::min)((std::min)(fars.x, fars.y), fars.z);
	if (min <= max)
	{
		if (min * max < 0.0f)
		{
			_t = 0.0f;
			_n = Normalize(-_a.mStart);
			return true;
		}
		if (min >= T::kMinT && min <= T::kMaxT &&
			max >= T::kMinT && max <= T::kMaxT)
		{
			_t = min;

			Vector3 point = _a.mStart + min * ab;
			Vector3 center = (_b.mMax + _b.mMin) * 0.5f;
			Vector3 size = (_b.mMax - _b.mMin) * 0.5f;

			Vector3 dir = point - center;
			_n.x = (std::abs(dir.x) >= size.x - MyMath::kEpsilon) ? MyMath::Sign(dir.x) : 0.0f;
			_n.y = (std::abs(dir.y) >= size.y - MyMath::kEpsilon) ? MyMath::Sign(dir.y) : 0.0f;
			_n.z = (std::abs(dir.z) >= size.z - MyMath::kEpsilon) ? MyMath::Sign(dir.z) : 0.0f;
			_n.Normalize();

			return true;
		}
	}
	return false;
}

template <typename T>
bool Intersect(const T& _a, const OBB& _b, float& _t, Vector3& _n)
{
	Matrix4 inv = _b.GetInverse();
	T a(_a.mStart * inv, _a.mEnd * inv);
	AABB b(-_b.mSize, _b.mSize);
	if (Intersect(a, b, _t, _n))
	{
		inv.m[3][0] = 0.0f;
		inv.m[3][1] = 0.0f;
		inv.m[3][2] = 0.0f;
		_n *= Transpose(inv);
		_n.Normalize();
		return true;
	}
	else
	{
		return false;
	}
}

// Plane
bool Intersect(const Plane& _a, const Sphere& _b);
// Sphere
bool Intersect(const Sphere& _a, const Sphere& _b, CollisionInfo& _info);
bool Intersect(const Sphere& _a, const AABB& _b, CollisionInfo& _info);
bool Intersect(const Sphere& _a, const OBB& _b, CollisionInfo& _info);
// AABB
bool Intersect(const AABB& _a, const AABB& _b, CollisionInfo& _info);
bool Intersect(const AABB& _a, const OBB& _b, CollisionInfo& _info);
// OBB
bool Intersect(const OBB& _a, const OBB& _b, CollisionInfo& _info);
