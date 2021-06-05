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
	// ��ǰActor��λ��
	Vector3 cameraPos = mOwner->GetPosition();

	//���¸�����
	mPitch += mPitchSpeed * deltaTime;
	// Clamp
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// ƫת
	Quaternion q(mOwner->GetRight(), mPitch);

	// ��ǰ���ӽ�ƫת
	Vector3 viewForward = Vector3::Transform(
		mOwner->GetForward(), q);
	// ���ǰ����Ŀ��λ��
	Vector3 target = cameraPos + viewForward * 100.0f;
	// ��ת��������
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// ����۲����
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}