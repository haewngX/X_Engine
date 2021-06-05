#pragma once
#include "CameraComponent.h"

class FollowCamera : public CameraComponent
{
public:
	FollowCamera(class Actor* owner);

	void Update(float deltaTime) override;
	
	void SnapToIdeal();

	void SetHorzDist(float dist) { mHorzDist = dist; }
	void SetVertDist(float dist) { mVertDist = dist; }
	void SetTargetDist(float dist) { mTargetDist = dist; }
	void SetSpringConstant(float spring) { mSpringConstant = spring; }

	float GetPitchSpeed() const { return mPitchSpeed; }
	float GetMaxPitch() const { return mMaxPitch; }

	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	void SetMaxPitch(float pitch) { mMaxPitch = pitch; }
private:
	Vector3 ComputeCameraPos() const;

	// 相机的实际位置
	Vector3 mActualPos;
	// 相机的实际速度
	Vector3 mVelocity;
	// 相机在角色后方的水平距离
	float mHorzDist;
	// 相机在角色上方的垂直距离
	float mVertDist;
	// 跟拍相机的目标点位置（在角色前方）
	float mTargetDist;
	// Spring constant (higher is more stiff)
	float mSpringConstant;

	// Rotation/sec speed of pitch
	float mPitchSpeed;
	// Maximum pitch deviation from forward
	float mMaxPitch;
	// Current pitch
	float mPitch;
};
