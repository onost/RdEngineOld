#pragma once
#include "Matrix4.h"
#include "MyMath.h"
#include "Vector3.h"

class Actor;
class Collider;

struct CollisionInfo
{
	Vector3 mNormal;
	float mDepth;
};

// レイキャスト用
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
	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = std::numeric_limits<float>::lowest();
	static constexpr float kMaxT = (std::numeric_limits<float>::max)();

	Line(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}
	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}
};

// 半直線
struct Ray
{
	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = 0.0f;
	static constexpr float kMaxT = (std::numeric_limits<float>::max)();

	Ray(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}
	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}
};

// 線分
struct Segment
{
	Vector3 mStart;
	Vector3 mEnd;
	static constexpr float kMinT = 0.0f;
	static constexpr float kMaxT = 1.0f;

	Segment(const Vector3& start, const Vector3& end)
		: mStart(start)
		, mEnd(end)
	{}
	Vector3 GetPoint(float t) const
	{
		return mStart + (mEnd - mStart) * t;
	}
};

// 三角形
struct Triangle
{
	Vector3 mVertices[3];

	Triangle()
	{}
	Triangle(const Vector3& a, const Vector3& b, const Vector3& c)
		: mVertices{ a,b,c }
	{}

	Vector3 GetNormal()
	{
		Vector3 a = mVertices[0];
		Vector3 b = mVertices[1];
		Vector3 c = mVertices[2];
		return Normalize(Cross(b - a, c - a));
	}
};

// 平面
struct Plane
{
	Vector3 mNormal;
	float mDistance;

	Plane(const Vector3& p, const Vector3& normal)
		: mNormal(normal)
	{
		mDistance = Dot(p, normal);
	}
	Plane(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		mNormal = Normalize(Cross(b - a, c - a));
		mDistance = Dot(a, mNormal);
	}
	// 三角形から
	Plane(const Triangle& triangle)
	{
		Vector3 a = triangle.mVertices[0];
		Vector3 b = triangle.mVertices[1];
		Vector3 c = triangle.mVertices[2];
		mNormal = Normalize(Cross(b - a, c - a));
		mDistance = Dot(a, mNormal);
	}
};

// 球
struct Sphere
{
	Vector3 mCenter;
	float mRadius;

	Sphere(const Vector3& center, float radius)
		: mCenter(center)
		, mRadius(radius)
	{}
};

struct AABB
{
	Vector3 mMin;
	Vector3 mMax;

	AABB(const Vector3& min, const Vector3& max)
		: mMin(min)
		, mMax(max)
	{}
};

struct OBB
{
	Vector3 mCenter;
	Vector3 mAxis[3];
	Vector3 mSize;

	OBB()
		: mCenter(Vector3::kZero)
		, mSize(Vector3::kZero)
	{
		mAxis[0] = Vector3(1.0f, 0.0f, 0.0f);
		mAxis[1] = Vector3(0.0f, 1.0f, 0.0f);
		mAxis[2] = Vector3(0.0f, 0.0f, 1.0f);
	}
	OBB(const Vector3& center, const Vector3& size)
		: mCenter(center)
		, mSize(size)
	{
		mAxis[0] = Vector3(1.0f, 0.0f, 0.0f);
		mAxis[1] = Vector3(0.0f, 1.0f, 0.0f);
		mAxis[2] = Vector3(0.0f, 0.0f, 1.0f);
	}
	Matrix4 CreateInverse() const
	{
		Matrix4 result = Matrix4::kIdentity;
		result.m[0][0] = mAxis[0].x;
		result.m[0][1] = mAxis[1].x;
		result.m[0][2] = mAxis[2].x;
		result.m[1][0] = mAxis[0].y;
		result.m[1][1] = mAxis[1].y;
		result.m[1][2] = mAxis[2].y;
		result.m[2][0] = mAxis[0].z;
		result.m[2][1] = mAxis[1].z;
		result.m[2][2] = mAxis[2].z;
		result.m[3][0] = -Dot(mCenter, mAxis[0]);
		result.m[3][1] = -Dot(mCenter, mAxis[1]);
		result.m[3][2] = -Dot(mCenter, mAxis[2]);
		return result;
	}
};

// カプセル
struct Capsule
{
	Segment mSegment;
	float mRadius;

	Capsule()
		: mSegment({ Vector3::kZero,Vector3::kZero })
		, mRadius(0.0f)
	{}
	Capsule(const Segment& segment, float radius)
		: mSegment(segment)
		, mRadius(radius)
	{}
};



// 点と線
template <class TLine>
Vector3 Closest(const Vector3& a, const TLine& b, float* outT = nullptr)
{
	Vector3 ab = b.mEnd - b.mStart;
	Vector3 ac = a - b.mStart;
	float t = Dot(ac, ab) / Dot(ab, ab);
	t = MyMath::Clamp(t, TLine::kMinT, TLine::kMaxT);
	if (outT)
	{
		*outT = t;
	}
	return b.GetPoint(t);
}

template <class TLine>
float Distance(const Vector3& a, const TLine& b, float* outT = nullptr)
{
	Vector3 p = Closest(a, b, outT);
	return Length(p - a);
}

Vector3 Closest(const Vector3& a, const AABB& b);
float Distance(const Line& a, const Line& b, float* outT1 = nullptr, float* outT2 = nullptr);
float Distance(const Segment& a, const Segment& b, float* outT1 = nullptr, float* outT2 = nullptr);
float Distance(const Vector3& a, const Plane& b);
float Distance(const Segment& a, const Plane& b);
//bool Contain(const Vector3& a, const Capsule& b);

// ==================================================
// Intersect
// ==================================================

// 線と三角形
template <class TLine>
bool Intersect(const TLine& a, const Triangle& b, float& outT)
{
	Plane plane = Plane(b);
	if (Intersect(a, plane, outT))
	{
		Vector3 p = a.GetPoint(outT);
		Vector3 c1 = Cross(b.mVertices[1] - b.mVertices[0], p - b.mVertices[1]);
		Vector3 c2 = Cross(b.mVertices[2] - b.mVertices[1], p - b.mVertices[2]);
		Vector3 c3 = Cross(b.mVertices[0] - b.mVertices[2], p - b.mVertices[0]);
		if (Dot(c1, plane.mNormal) >= 0.0f &&
			Dot(c2, plane.mNormal) >= 0.0f &&
			Dot(c3, plane.mNormal) >= 0.0f)
		{
			if (outT >= TLine::kMinT && outT <= TLine::kMaxT &&
				outT >= TLine::kMinT && outT <= TLine::kMaxT)
			{
				return true;
			}
		}
	}
	return false;
}

// 線と平面
template <class TLine>
bool Intersect(const TLine& a, const Plane& b, float& outT)
{
	float d = Dot(b.mNormal, a.mEnd - a.mStart);
	if (fabs(d) < MyMath::kEpsilon)
	{
		return false;
	}
	outT = (b.mDistance - Dot(a.mStart, b.mNormal)) / d;
	if (outT >= TLine::kMinT && outT <= TLine::kMaxT)
	{
		return true;
	}
	return false;
}

// 線と球
template <class TLine>
bool Intersect(const TLine& tLine, const Sphere& sphere, float& outT, Vector3& normal)
{
	Vector3 x = tLine.mStart - sphere.mCenter;
	Vector3 y = tLine.mEnd - tLine.mStart;
	{
		float a = Dot(y, y);
		float b = 2.0f * Dot(x, y);
		float c = Dot(x, x) - sphere.mRadius * sphere.mRadius;
		float d = b * b - 4.0f * a * c;
		if (d < 0.0f)
		{
			return false;
		}
		else
		{
			d = sqrtf(d);
			float min = (-b - d) / (2.0f * a);
			float max = (-b + d) / (2.0f * a);
			if (min >= TLine::kMinT && min <= TLine::kMaxT)
			{
				outT = min;
				Vector3 p = tLine.mStart + min * y;
				normal = Normalize(p - sphere.mCenter);
				return true;
			}
			else if (max >= TLine::kMinT && max <= TLine::kMaxT)
			{
				outT = max;
				Vector3 p = tLine.mStart + max * y;
				normal = Normalize(p - sphere.mCenter);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

// 線とAABB
template <class TLine>
bool Intersect(const TLine& a, const AABB& b, float& outT, Vector3& normal)
{
	Vector3 v = a.mEnd - a.mStart;
	Vector3 mins;
	mins.x = (-a.mStart.x + b.mMin.x) / v.x;
	mins.y = (-a.mStart.y + b.mMin.y) / v.y;
	mins.z = (-a.mStart.z + b.mMin.z) / v.z;
	Vector3 maxes;
	maxes.x = (-a.mStart.x + b.mMax.x) / v.x;
	maxes.y = (-a.mStart.y + b.mMax.y) / v.y;
	maxes.z = (-a.mStart.z + b.mMax.z) / v.z;
	Vector3 nears;
	nears.x = MyMath::Min(mins.x, maxes.x);
	nears.y = MyMath::Min(mins.y, maxes.y);
	nears.z = MyMath::Min(mins.z, maxes.z);
	Vector3 fars;
	fars.x = MyMath::Max(mins.x, maxes.x);
	fars.y = MyMath::Max(mins.y, maxes.y);
	fars.z = MyMath::Max(mins.z, maxes.z);
	float min = MyMath::Max(MyMath::Max(nears.x, nears.y), nears.z);
	float max = MyMath::Min(MyMath::Min(fars.x, fars.y), fars.z);
	if (min <= max)
	{
		if (min * max < 0.0f)
		{
			outT = 0.0f;
			normal = Normalize(-a.mStart);
			return true;
		}
		if (min >= TLine::kMinT && min <= TLine::kMaxT &&
			max >= TLine::kMinT && max <= TLine::kMaxT)
		{
			outT = min;
			Vector3 p = a.mStart + min * v;
			Vector3 center = (b.mMax + b.mMin) * 0.5f;
			Vector3 size = (b.mMax - b.mMin) * 0.5f;
			Vector3 direction = p - center;
			normal = Vector3(
				(fabsf(direction.x) >= size.x - MyMath::kEpsilon) ? MyMath::Sign(direction.x) : 0.0f,
				(fabsf(direction.y) >= size.y - MyMath::kEpsilon) ? MyMath::Sign(direction.y) : 0.0f,
				(fabsf(direction.z) >= size.z - MyMath::kEpsilon) ? MyMath::Sign(direction.z) : 0.0f);
			normal.Normalize();
			return true;
		}
	}
	return false;
}

// 線とOBB
template <class TLine>
bool Intersect(const TLine& a, const OBB& b, float& outT, Vector3& normal)
{
	Matrix4 inverse = b.CreateInverse();
	TLine tLine = { a.mStart * inverse,a.mEnd * inverse };
	AABB aabb = { -b.mSize,b.mSize };
	if (Intersect(tLine, aabb, outT, normal))
	{
		inverse.m[3][0] = inverse.m[3][1] = inverse.m[3][2] = 0.0f;
		normal *= Transpose(inverse);
		normal.Normalize();
		return true;
	}
	else
	{
		return false;
	}
}

// Plane
bool Intersect(const Plane& a, const Sphere& b);
//bool Intersect(const Plane& a, const Capsule& b);
// Sphere
bool Intersect(const Sphere& a, const Sphere& b, CollisionInfo& info);
bool Intersect(const Sphere& a, const AABB& b, CollisionInfo& info);
bool Intersect(const Sphere& a, const OBB& b, CollisionInfo& info);
//bool Intersect(const Sphere& a, const Capsule& b);
// AABB
bool Intersect(const AABB& a, const AABB& b, CollisionInfo& info);
bool Intersect(const AABB& a, const OBB& b, CollisionInfo& info);
//bool Intersect(const AABB& a, const Capsule& b)
// OBB
bool Intersect(const OBB& a, const OBB& b, CollisionInfo& info);
//bool Intersect(const OBB& a, const Capsule& b);
// Capsule
bool Intersect(const Capsule& a, const Capsule& b);
