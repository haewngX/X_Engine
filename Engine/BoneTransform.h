#pragma once
#include "Math.h"

class BoneTransform
{
public:
	Quaternion mRotation; //������ת
	Vector3 mTranslation; //����ƽ��

	Matrix4 ToMatrix() const; //��BoneTransformת��Ϊ����

	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};
