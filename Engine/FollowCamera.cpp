#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
	:CameraComponent(owner)
	,mHorzDist(200.0f)
	,mVertDist(200.0f)
	,mTargetDist(80.0f)
	,mSpringConstant(128.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// 根据弹簧常数计算阻尼 
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);
	// 计算理想的相机位置
	Vector3 idealPos = ComputeCameraPos();
	// Compute difference between actual and ideal
	Vector3 diff = mActualPos - idealPos;
	// Compute acceleration of spring
	Vector3 acel = -mSpringConstant * diff -
		dampening * mVelocity;
	// Update velocity
	mVelocity += acel * deltaTime;
	// Update actual camera position
	mActualPos += mVelocity * deltaTime;

	// Target is target dist in front of owning actor
	Vector3 target = mOwner->GetPosition() +
		mOwner->GetForward() * mTargetDist;


	//// Also rotate up by pitch quaternion
	//Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);

	


}

void FollowCamera::SnapToIdeal()
{
	// Set actual position to ideal
	mActualPos = ComputeCameraPos();
	// Zero velocity
	mVelocity = Vector3::Zero;
	// Compute target and view
	Vector3 target = mOwner->GetPosition() +
		mOwner->GetForward() * mTargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// 设置相机的位置
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}
