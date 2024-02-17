#include "Collision.h"

// 点とAABB
Vector3 Closest(const Vector3& a, const AABB& b)
{
	return Vector3(
		MyMath::Clamp(a.x, b.mMin.x, b.mMax.x),
		MyMath::Clamp(a.y, b.mMin.y, b.mMax.y),
		MyMath::Clamp(a.z, b.mMin.z, b.mMax.z));
}

// 線と線
float Distance(const Line& a, const Line& b, float* outT1, float* outT2)
{
	Vector3 v1 = a.mEnd - a.mStart;
	Vector3 v2 = b.mEnd - b.mStart;
	float v2v2 = Dot(v2, v2);
	float v1v2 = Dot(v1, v2);
	float d = Dot(v1, v1) * v2v2 - v1v2 * v1v2;
	float t1 = 0.0f;
	float t2 = 0.0f;
	if (fabs(d) <= MyMath::kEpsilon)
	{
		float dist = Distance(a.mStart, b, &t2);
		if (outT1)
		{
			*outT1 = 0.0f;
		}
		if (outT2)
		{
			*outT2 = t2;
		}
		return dist;
	}
	Vector3 stos = a.mStart - b.mStart;
	t1 = v1v2 * Dot(v2, stos) - v2v2 * Dot(v1, stos) / d;
	Vector3 p1 = a.GetPoint(t1);
	t2 = Dot(v2, p1 - b.mStart) / v2v2;
	Vector3 p2 = b.GetPoint(t2);
	if (outT1)
	{
		*outT1 = t1;
	}
	if (outT2)
	{
		*outT2 = t2;
	}
	return Length(p2 - p1);
}

// 線分と線分
float Distance(const Segment& a, const Segment& b, float* outT1, float* outT2)
{
	Vector3 v1 = a.mEnd - a.mStart;
	Vector3 v2 = b.mEnd - b.mStart;
	Line l1 = { a.mStart,a.mEnd };
	Line l2 = { b.mStart,b.mEnd };
	float t1 = 0.0f;
	float t2 = 0.0f;
	if (fabs(1.0f - Dot(v1, v2)) < MyMath::kEpsilon)
	{
		float dist = Distance(a.mStart, l2, &t2);
		if (t2 >= 0.0f && t2 <= 1.0f)
		{
			if (outT1)
			{
				*outT1 = 0.0f;
			}
			if (outT2)
			{
				*outT2 = t2;
			}
			return dist;
		}
	}
	else
	{
		float dist = Distance(l1, l2, &t1, &t2);
		if (t1 >= 0.0f && t1 <= 1.0f &&
			t2 >= 0.0f && t2 <= 1.0f)
		{
			if (outT1)
			{
				*outT1 = 0.0f;
			}
			if (outT2)
			{
				*outT2 = t2;
			}
			return dist;
		}
	}
	t1 = MyMath::Clamp(t1, Segment::kMinT, Segment::kMaxT);
	Vector3 p1 = a.GetPoint(t1);
	float dist = Distance(p1, l2, &t2);
	if (t2 >= 0.0f && t2 <= 1.0f)
	{
		if (outT1)
		{
			*outT1 = t1;
		}
		if (outT2)
		{
			*outT2 = t2;
		}
		return dist;
	}
	t2 = MyMath::Clamp(t2, Segment::kMinT, Segment::kMaxT);
	Vector3 p2 = b.GetPoint(t2);
	dist = Distance(p2, l1, &t1);
	if (t1 >= 0.0f && t1 <= 1.0f)
	{
		if (outT1)
		{
			*outT1 = t1;
		}
		if (outT2)
		{
			*outT2 = t2;
		}
		return dist;
	}
	t1 = MyMath::Clamp(t1, Segment::kMinT, Segment::kMaxT);
	p1 = a.GetPoint(t1);
	if (outT1)
	{
		*outT1 = t1;
	}
	if (outT2)
	{
		*outT2 = t2;
	}
	return Length(p2 - p1);
}

// 点と平面
float Distance(const Vector3& a, const Plane& b)
{
	return Dot(b.mNormal, a) - b.mDistance;
}

// 線分と平面
float Distance(const Segment& a, const Plane& b)
{
	float d1 = Dot(a.mStart, b.mNormal) - b.mDistance;
	float d2 = Dot(a.mEnd, b.mNormal) - b.mDistance;
	if (d1 * d2 <= 0.0f)
	{
		return 0.0f;
	}
	if (fabs(d1) < fabs(d2))
	{
		return d1;
	}
	else
	{
		return d2;
	}
}

// 点とカプセル
bool Contain(const Vector3& a, const Capsule& b)
{
	float d = Distance(a, b.mSegment);
	return d <= b.mRadius;
}

// ==================================================
// Plane
// ==================================================

bool Intersect(const Plane& a, const Sphere& b)
{
	float d = Distance(b.mCenter, a);
	return d * d <= b.mRadius * b.mRadius;
}

bool Intersect(const Plane& a, const Capsule& b)
{
	float d = Distance(b.mSegment, a);
	return d * d <= b.mRadius * b.mRadius;
}

// ==================================================
// Sphere
// ==================================================

// 球と球
bool Intersect(const Sphere& a, const Sphere& b, CollisionInfo& info)
{
	Vector3 d = a.mCenter - b.mCenter;
	float r = a.mRadius + b.mRadius;
	float len = Length(d);
	if (len != 0.0f)
	{
		info.mNormal = d / len;
	}
	else
	{
		info.mNormal = Vector3::kZero;
	}
	info.mDepth = r - len;
	return LengthSq(d) <= r * r;
}

// 球とAABB
bool Intersect(const Sphere& a, const AABB& b, CollisionInfo& info)
{
	Vector3 p = Vector3(
		MyMath::Clamp(a.mCenter.x, b.mMin.x, b.mMax.x),
		MyMath::Clamp(a.mCenter.y, b.mMin.y, b.mMax.y),
		MyMath::Clamp(a.mCenter.z, b.mMin.z, b.mMax.z));
	float d = Length(p - a.mCenter);
	if (d <= a.mRadius)
	{
		info.mNormal = Normalize(a.mCenter - p);
		info.mDepth = a.mRadius - d;
		return true;
	}
	return false;
}

// 球とOBB
bool Intersect(const Sphere& a, const OBB& b, CollisionInfo& info)
{
	Matrix4 inverse = b.CreateInverse();
	Sphere sphere = { a.mCenter * inverse, a.mRadius };
	AABB aabb = { -b.mSize,b.mSize };
	if (Intersect(sphere, aabb, info))
	{
		Matrix4 m = Inverse(inverse);
		m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
		info.mNormal *= m;
		return true;
	}
	return false;
}

// 球とカプセル
bool Intersect(const Sphere& a, const Capsule& b)
{
	float d = Distance(a.mCenter, b.mSegment);
	if (d <= a.mRadius + b.mRadius)
	{
		return true;
	}
	return false;
}

// ==================================================
// AABB
// ==================================================

// AABBとAABB
bool Intersect(const AABB& a, const AABB& b, CollisionInfo& info)
{
	if (a.mMin.x <= b.mMax.x &&
		a.mMax.x >= b.mMin.x &&
		a.mMin.y <= b.mMax.y &&
		a.mMax.y >= b.mMin.y &&
		a.mMin.z <= b.mMax.z &&
		a.mMax.z >= b.mMin.z)
	{
		float x = MyMath::Min(a.mMax.x - b.mMin.x, b.mMax.x - a.mMin.x);
		float y = MyMath::Min(a.mMax.y - b.mMin.y, b.mMax.y - a.mMin.y);
		float z = MyMath::Min(a.mMax.z - b.mMin.z, b.mMax.z - a.mMin.z);
		float minOverlap = MyMath::kInfinity;
		if (x < minOverlap)
		{
			minOverlap = x;
			if (a.mMax.x - b.mMin.x < b.mMax.x - a.mMin.x)
			{
				info.mNormal = Vector3(-1.0f, 0.0f, 0.0f);
			}
			else
			{
				info.mNormal = Vector3(1.0f, 0.0f, 0.0f);
			}
		}
		if (y < minOverlap)
		{
			minOverlap = y;
			if (a.mMax.y - b.mMin.y < b.mMax.y - a.mMin.y)
			{
				info.mNormal = Vector3(0.0f, -1.0f, 0.0f);
			}
			else
			{
				info.mNormal = Vector3(0.0f, 1.0f, 0.0f);
			}
		}
		if (z < minOverlap)
		{
			minOverlap = z;
			if (a.mMax.z - b.mMin.z < b.mMax.z - a.mMin.z)
			{
				info.mNormal = Vector3(0.0f, 0.0f, -1.0f);
			}
			else
			{
				info.mNormal = Vector3(0.0f, 0.0f, 1.0f);
			}
		}
		info.mDepth = minOverlap;
		return true;
	}
	return false;
}

// AABBとOBB
bool Intersect(const AABB& a, const OBB& b, CollisionInfo& info)
{
	Vector3 size = (a.mMax - a.mMin) / 2.0f;
	OBB obb;
	obb.mCenter = a.mMin + size;
	//obb.mAxis;
	obb.mSize = size;
	if (Intersect(obb, b, info))
	{
		return true;
	}
	return false;
}

// ==================================================
// OBB
// ==================================================

// OBBとOBB
bool Intersect(const OBB& a, const OBB& b, CollisionInfo& info)
{
	// 分離軸
	Vector3 axes[15];
	axes[0] = a.mAxis[0];
	axes[1] = a.mAxis[1];
	axes[2] = a.mAxis[2];
	axes[3] = b.mAxis[0];
	axes[4] = b.mAxis[1];
	axes[5] = b.mAxis[2];
	axes[6] = Normalize(Cross(a.mAxis[0], b.mAxis[0]));
	axes[7] = Normalize(Cross(a.mAxis[0], b.mAxis[1]));
	axes[8] = Normalize(Cross(a.mAxis[0], b.mAxis[2]));
	axes[9] = Normalize(Cross(a.mAxis[1], b.mAxis[0]));
	axes[10] = Normalize(Cross(a.mAxis[1], b.mAxis[1]));
	axes[11] = Normalize(Cross(a.mAxis[1], b.mAxis[2]));
	axes[12] = Normalize(Cross(a.mAxis[2], b.mAxis[0]));
	axes[13] = Normalize(Cross(a.mAxis[2], b.mAxis[1]));
	axes[14] = Normalize(Cross(a.mAxis[2], b.mAxis[2]));
	Vector3 d1[3] =
	{
		a.mAxis[0] * a.mSize.x,
		a.mAxis[1] * a.mSize.y,
		a.mAxis[2] * a.mSize.z
	};
	Vector3 d2[3] =
	{
		b.mAxis[0] * b.mSize.x,
		b.mAxis[1] * b.mSize.y,
		b.mAxis[2] * b.mSize.z
	};
	Vector3 c1[8] =
	{
		a.mCenter + d1[0] + d1[1] + d1[2],
		a.mCenter + d1[0] + d1[1] - d1[2],
		a.mCenter + d1[0] - d1[1] + d1[2],
		a.mCenter + d1[0] - d1[1] - d1[2],
		a.mCenter - d1[0] + d1[1] + d1[2],
		a.mCenter - d1[0] + d1[1] - d1[2],
		a.mCenter - d1[0] - d1[1] + d1[2],
		a.mCenter - d1[0] - d1[1] - d1[2]
	};
	Vector3 c2[8] =
	{
		b.mCenter + d2[0] + d2[1] + d2[2],
		b.mCenter + d2[0] + d2[1] - d2[2],
		b.mCenter + d2[0] - d2[1] + d2[2],
		b.mCenter + d2[0] - d2[1] - d2[2],
		b.mCenter - d2[0] + d2[1] + d2[2],
		b.mCenter - d2[0] + d2[1] - d2[2],
		b.mCenter - d2[0] - d2[1] + d2[2],
		b.mCenter - d2[0] - d2[1] - d2[2]
	};
	float minOverlap = MyMath::kInfinity;
	Vector3 minAxis;
	for (const auto& axis : axes)
	{
		if (axis.x > -MyMath::kEpsilon && axis.x < MyMath::kEpsilon &&
			axis.y > -MyMath::kEpsilon && axis.y < MyMath::kEpsilon &&
			axis.z > -MyMath::kEpsilon && axis.z < MyMath::kEpsilon)
		{
			continue;
		}
		float min1 = (std::numeric_limits<float>::max)();
		float max1 = std::numeric_limits<float>::lowest();
		float min2 = min1;
		float max2 = max1;
		for (uint32_t i = 0; i < 8; ++i)
		{
			float dist1 = Dot(c1[i], axis);
			min1 = MyMath::Min(dist1, min1);
			max1 = MyMath::Max(dist1, max1);
			float dist2 = Dot(c2[i], axis);
			min2 = MyMath::Min(dist2, min2);
			max2 = MyMath::Max(dist2, max2);
		}
		float overlap = (max1 - min1) + (max2 - min2) - (MyMath::Max(max1, max2) - MyMath::Min(min1, min2));
		if (overlap < 0.0f)
		{
			return false;
		}
		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			minAxis = axis;
		}
	}
	info.mNormal = Normalize(minAxis);
	if (Dot(a.mCenter - b.mCenter, info.mNormal) < 0.0f)
	{
		info.mNormal = -info.mNormal;
	}
	info.mDepth = minOverlap;
	return true;
}

/*
// OBBとカプセル
bool Intersect(const OBB& a, const Capsule& b)
{
	Matrix4 inverse = a.CreateInverse();
	AABB aabb = { -a.mSize,a.mSize };
	Capsule capsule;
	capsule.mSegment.mStart = b.mSegment.mStart * inverse;
	capsule.mSegment.mEnd = b.mSegment.mEnd * inverse;
	capsule.mRadius = b.mRadius;
	if (Intersect(aabb, capsule))
	{
		return true;
	}
	return false;
}
*/

// ==================================================
// Capsule
// ==================================================

// カプセルとカプセル
bool Intersect(const Capsule& a, const Capsule& b)
{
	float d = Distance(a.mSegment, b.mSegment);
	if (d < a.mRadius + b.mRadius)
	{
		return true;
	}
	return false;
}
