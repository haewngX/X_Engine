#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	:CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
{
}

void FPSCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// 当前Actor的位置
	Vector3 cameraPos = mOwner->GetPosition();

	//更新俯仰角
	mPitch += mPitchSpeed * deltaTime;
	// Clamp
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// 偏转
	Quaternion q(mOwner->GetRight(), mPitch);

	// 向前的视角偏转
	Vector3 viewForward = Vector3::Transform(
		mOwner->GetForward(), q);
	// 相机前方的目标位置
	Vector3 target = cameraPos + viewForward * 100.0f;
	// 旋转向上向量
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// 构造观测矩阵
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}