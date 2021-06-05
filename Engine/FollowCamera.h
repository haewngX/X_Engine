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

	// �����ʵ��λ��
	Vector3 mActualPos;
	// �����ʵ���ٶ�
	Vector3 mVelocity;
	// ����ڽ�ɫ�󷽵�ˮƽ����
	float mHorzDist;
	// ����ڽ�ɫ�Ϸ��Ĵ�ֱ����
	float mVertDist;
	// ���������Ŀ���λ�ã��ڽ�ɫǰ����
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
