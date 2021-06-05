#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent : public MeshComponent
{
public:
	SkeletalMeshComponent(class Actor* owner);

	void Draw(class Shader* shader) override;

	void Update(float deltaTime) override;


	void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

	// ���Ŷ��������ض����ĳ���
	float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
	void ComputeMatrixPalette();

	MatrixPalette mPalette;
	const class Skeleton* mSkeleton;
	const class Animation* mAnimation;
	float mAnimPlayRate;
	float mAnimTime;
};
