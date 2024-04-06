#include "Collision.h"

// ==================================================
// Closest
// ==================================================

Vector3 Closest(const Vector3& _a, const AABB& _b)
{
	return Vector3(
		std::clamp(_a.x, _b.mMin.x, _b.mMax.x),
		std::clamp(_a.y, _b.mMin.y, _b.mMax.y),
		std::clamp(_a.z, _b.mMin.z, _b.mMax.z));
}

// ==================================================
// Distance
// ==================================================

float Distance(const Line& _a, const Line& _b, float& _t1, float& _t2)
{
	Vector3 v1 = _a.mEnd - _a.mStart;
	Vector3 v2 = _b.mEnd - _b.mStart;
	float v2v2 = Dot(v2, v2);
	float v1v2 = Dot(v1, v2);
	float d = Dot(v1, v1) * v2v2 - v1v2 * v1v2;
	if (std::abs(d) <= MyMath::kEpsilon)// 平行か？
	{
		_t1 = 0.0f;
		return Distance(_a.mStart, _b, _t2);
	}
	Vector3 diff = _a.mStart - _b.mStart;
	_t1 = v1v2 * Dot(v2, diff) - v2v2 * Dot(v1, diff) / d;
	Vector3 point1 = _a.GetPoint(_t1);
	_t2 = Dot(v2, point1 - _b.mStart) / v2v2;
	Vector3 point2 = _b.GetPoint(_t2);
	return Length(point2 - point1);
}

float Distance(const Segment& _a, const Segment& _b, float& _t1, float& _t2)
{
	Vector3 v1 = _a.mEnd - _a.mStart;
	Vector3 v2 = _b.mEnd - _b.mStart;
	Line line1(_a.mStart, _a.mEnd);
	Line line2(_b.mStart, _b.mEnd);
	if (std::abs(1.0f - Dot(v1, v2)) < MyMath::kEpsilon)// 平行か？
	{
		float dist = Distance(_a.mStart, line2, _t2);
		if (_t2 >= 0.0f && _t2 <= 1.0f)
		{
			_t1 = 0.0f;
			return dist;
		}
	}
	else
	{
		float dist = Distance(line1, line2, _t1, _t2);
		if (_t1 >= 0.0f && _t1 <= 1.0f &&
			_t2 >= 0.0f && _t2 <= 1.0f)
		{
			return dist;
		}
	}
	_t1 = std::clamp(_t1, 0.0f, 1.0f);
	Vector3 point1 = _a.GetPoint(_t1);
	float dist = Distance(point1, line2, _t2);
	if (_t2 >= 0.0f && _t2 <= 1.0f)
	{
		return dist;
	}
	_t2 = std::clamp(_t2, 0.0f, 1.0f);
	Vector3 point2 = _b.GetPoint(_t2);
	dist = Distance(point2, line1, _t1);
	if (_t1 >= 0.0f && _t1 <= 1.0f)
	{
		return dist;
	}
	_t1 = std::clamp(_t1, 0.0f, 1.0f);
	point1 = _a.GetPoint(_t1);
	return Length(point2 - point1);
}

float Distance(const Vector3& _a, const Plane& _b)
{
	return Dot(_a, _b.mNormal) - _b.mDistance;
}

float Distance(const Segment& _a, const Plane& _b)
{
	float dist1 = Dot(_a.mStart, _b.mNormal) - _b.mDistance;
	float dist2 = Dot(_a.mEnd, _b.mNormal) - _b.mDistance;
	if (dist1 * dist2 <= 0.0f)
	{
		return 0.0f;
	}
	if (std::abs(dist1) < std::abs(dist2))
	{
		return dist1;
	}
	else
	{
		return dist2;
	}
}

// ==================================================
// Intersect
// ==================================================

// Plane

bool Intersect(const Plane& _a, const Sphere& _b)
{
	float dist = Distance(_b.mCenter, _a);
	return dist * dist <= _b.mRadius * _b.mRadius;
}

// Sphere

bool Intersect(const Sphere& _a, const Sphere& _b, CollisionInfo& _info)
{
	Vector3 diff = _a.mCenter - _b.mCenter;
	float radius = _a.mRadius + _b.mRadius;
	float dist = Length(diff);
	if (dist >= MyMath::kEpsilon)
	{
		_info.mNormal = Normalize(diff);
	}
	else
	{
		_info.mNormal = Vector3::kZero;
	}
	_info.mDepth = radius - dist;
	return dist * dist <= radius * radius;
}

bool Intersect(const Sphere& _a, const AABB& _b, CollisionInfo& _info)
{
	Vector3 point = Vector3(
		std::clamp(_a.mCenter.x, _b.mMin.x, _b.mMax.x),
		std::clamp(_a.mCenter.y, _b.mMin.y, _b.mMax.y),
		std::clamp(_a.mCenter.z, _b.mMin.z, _b.mMax.z));
	float dist = Length(point - _a.mCenter);
	if (dist <= _a.mRadius)
	{
		_info.mNormal = Normalize(_a.mCenter - point);
		_info.mDepth = _a.mRadius - dist;
		return true;
	}
	return false;
}

bool Intersect(const Sphere& _a, const OBB& _b, CollisionInfo& _info)
{
	Matrix4 inv = _b.GetInverse();
	Sphere a(_a.mCenter * inv, _a.mRadius);
	AABB b(-_b.mSize, _b.mSize);
	if (Intersect(a, b, _info))
	{
		inv = Inverse(inv);
		inv.m[3][0] = 0.0f;
		inv.m[3][1] = 0.0f;
		inv.m[3][2] = 0.0f;
		_info.mNormal *= inv;
		return true;
	}
	return false;
}

// AABB

bool Intersect(const AABB& _a, const AABB& _b, CollisionInfo& _info)
{
	if (_a.mMin.x <= _b.mMax.x &&
		_a.mMax.x >= _b.mMin.x &&
		_a.mMin.y <= _b.mMax.y &&
		_a.mMax.y >= _b.mMin.y &&
		_a.mMin.z <= _b.mMax.z &&
		_a.mMax.z >= _b.mMin.z)
	{
		float x1 = _a.mMax.x - _b.mMin.x;
		float x2 = _b.mMax.x - _a.mMin.x;
		float y1 = _a.mMax.y - _b.mMin.y;
		float y2 = _b.mMax.y - _a.mMin.y;
		float z1 = _a.mMax.z - _b.mMin.z;
		float z2 = _b.mMax.z - _a.mMin.z;
		float x = (std::min)(x1, x2);
		float y = (std::min)(y1, y2);
		float z = (std::min)(z1, z2);
		float min = std::numeric_limits<float>::max();
		if (x < min)
		{
			min = x;
			_info.mNormal = Vector3(x1 > x2 ? 1.0f : -1.0f, 0.0f, 0.0f);
		}
		if (y < min)
		{
			min = y;
			_info.mNormal = Vector3(0.0f, y1 > y2 ? 1.0f : -1.0f, 0.0f);
		}
		if (z < min)
		{
			min = z;
			_info.mNormal = Vector3(0.0f, 0.0f, z1 > z2 ? 1.0f : -1.0f);
		}
		_info.mDepth = min;
		return true;
	}
	return false;
}

bool Intersect(const AABB& _a, const OBB& _b, CollisionInfo& _info)
{
	Vector3 center = (_a.mMax + _a.mMin) * 0.5f;
	Vector3 size = (_a.mMax - _a.mMin) * 0.5f;
	OBB a(center, size);
	if (Intersect(a, _b, _info))
	{
		return true;
	}
	return false;
}

// OBB

bool Intersect(const OBB& _a, const OBB& _b, CollisionInfo& _info)
{
	Vector3 axes[15];// 分離軸
	axes[0] = _a.mAxes[0];
	axes[1] = _a.mAxes[1];
	axes[2] = _a.mAxes[2];
	axes[3] = _a.mAxes[0];
	axes[4] = _a.mAxes[1];
	axes[5] = _a.mAxes[2];
	axes[6] = Normalize(Cross(_a.mAxes[0], _b.mAxes[0]));
	axes[7] = Normalize(Cross(_a.mAxes[0], _b.mAxes[1]));
	axes[8] = Normalize(Cross(_a.mAxes[0], _b.mAxes[2]));
	axes[9] = Normalize(Cross(_a.mAxes[1], _b.mAxes[0]));
	axes[10] = Normalize(Cross(_a.mAxes[1], _b.mAxes[1]));
	axes[11] = Normalize(Cross(_a.mAxes[1], _b.mAxes[2]));
	axes[12] = Normalize(Cross(_a.mAxes[2], _b.mAxes[0]));
	axes[13] = Normalize(Cross(_a.mAxes[2], _b.mAxes[1]));
	axes[14] = Normalize(Cross(_a.mAxes[2], _b.mAxes[2]));
	Vector3 dir1[3] =
	{
		_a.mAxes[0] * _a.mSize.x,
		_a.mAxes[1] * _a.mSize.y,
		_a.mAxes[2] * _a.mSize.z
	};
	Vector3 dir2[3] =
	{
		_b.mAxes[0] * _b.mSize.x,
		_b.mAxes[1] * _b.mSize.y,
		_b.mAxes[2] * _b.mSize.z
	};
	Vector3 corner1[8] =
	{
		_a.mCenter + dir1[0] + dir1[1] + dir1[2],
		_a.mCenter + dir1[0] + dir1[1] - dir1[2],
		_a.mCenter + dir1[0] - dir1[1] + dir1[2],
		_a.mCenter + dir1[0] - dir1[1] - dir1[2],
		_a.mCenter - dir1[0] + dir1[1] + dir1[2],
		_a.mCenter - dir1[0] + dir1[1] - dir1[2],
		_a.mCenter - dir1[0] - dir1[1] + dir1[2],
		_a.mCenter - dir1[0] - dir1[1] - dir1[2]
	};
	Vector3 corner2[8] =
	{
		_b.mCenter + dir2[0] + dir2[1] + dir2[2],
		_b.mCenter + dir2[0] + dir2[1] - dir2[2],
		_b.mCenter + dir2[0] - dir2[1] + dir2[2],
		_b.mCenter + dir2[0] - dir2[1] - dir2[2],
		_b.mCenter - dir2[0] + dir2[1] + dir2[2],
		_b.mCenter - dir2[0] + dir2[1] - dir2[2],
		_b.mCenter - dir2[0] - dir2[1] + dir2[2],
		_b.mCenter - dir2[0] - dir2[1] - dir2[2]
	};
	Vector3 minNormal;
	float minDepth = std::numeric_limits<float>::max();
	for (auto& axis : axes)
	{
		if (axis.x > -MyMath::kEpsilon && axis.x < MyMath::kEpsilon &&
			axis.y > -MyMath::kEpsilon && axis.y < MyMath::kEpsilon &&
			axis.z > -MyMath::kEpsilon && axis.z < MyMath::kEpsilon)
		{
			continue;
		}
		float min1 = std::numeric_limits<float>::max();
		float max1 = std::numeric_limits<float>::lowest();
		float min2 = min1;
		float max2 = max1;
		for (uint32_t i = 0; i < 8; ++i)
		{
			float dist1 = Dot(corner1[i], axis);
			min1 = std::min(dist1, min1);
			max1 = std::max(dist1, max1);
			float dist2 = Dot(corner2[i], axis);
			min2 = std::min(dist2, min2);
			max2 = std::max(dist2, max2);
		}
		float min = (max1 - min1) + (max2 - min2) - (std::max(max1, max2) - std::min(min1, min2));
		if (min < 0.0f)
		{
			return false;
		}
		if (min < minDepth)
		{
			minNormal = axis;
			minDepth = min;
		}
	}
	_info.mNormal = Normalize(minNormal);
	if (Dot(_a.mCenter - _b.mCenter, _info.mNormal) < 0.0f)
	{
		_info.mNormal = -_info.mNormal;
	}
	_info.mDepth = minDepth;
	return true;
}
