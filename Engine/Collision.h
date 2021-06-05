#pragma once
#include "Math.h"
#include <vector>

//线段
struct LineSegment
{
	LineSegment(const Vector3& start, const Vector3& end);
	// 获取线段上的点 where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	// 获取点到线段的最小距离
	float MinDistSq(const Vector3& point) const;
	// Get MinDistSq between two line segments
	static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector3 mStart;
	Vector3 mEnd;
};

//平面
struct Plane
{
	//从法线和到原点的最小符号距离构造平面
	Plane(const Vector3& normal, float d);
	// 从三个点构造平面
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	// 获取点到平面的有符号距离
	float SignedDist(const Vector3& point) const;

	Vector3 mNormal;
	float mD;
};

//球体
struct Sphere
{
	Sphere(const Vector3& center, float radius);
	bool Contains(const Vector3& point) const;

	Vector3 mCenter;
	float mRadius;
};

//轴对齐包围框
struct AABB
{
	AABB(const Vector3& min, const Vector3& max);
	// 根据给的点更新AABB 加载模型时使用
	void UpdateMinMax(const Vector3& point);
	// 旋转
	void Rotate(const Quaternion& q);
	bool Contains(const Vector3& point) const;
	float MinDistSq(const Vector3& point) const;

	Vector3 mMin;
	Vector3 mMax;
};

//定向包围框
struct OBB
{
	Vector3 mCenter;
	Quaternion mRotation;
	Vector3 mExtents;
};

//胶囊体
struct Capsule
{
	Capsule(const Vector3& start, const Vector3& end, float radius);
	// Get point along segment where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	bool Contains(const Vector3& point) const;

	LineSegment mSegment;
	float mRadius;
};

//凸多边形
struct ConvexPolygon
{
	bool Contains(const Vector2& point) const;
	// Vertices have a clockwise ordering
	std::vector<Vector2> mVertices;
};

// Intersection functions
bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const Capsule& a, const Capsule& b);
bool Intersect(const Sphere& s, const AABB& box);

bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& t);