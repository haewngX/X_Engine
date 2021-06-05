#pragma once
#include "Math.h"

class BoneTransform
{
public:
	Quaternion mRotation; //骨骼旋转
	Vector3 mTranslation; //骨骼平移

	Matrix4 ToMatrix() const; //将BoneTransform转化为矩阵

	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};
