#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
:Component(owner, updateOrder)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mUpSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetRotation();
		float angle = mAngularSpeed * deltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitZ, angle);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetRotation(rot);
	}
	
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed) || !Math::NearZero(mUpSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
		
		//ÌøÔ¾
		if (mOwner->mJumpState == Jump || mOwner->mJumpState == Fall)
		{
			mUpSpeed -= gravity * deltaTime;
			//printf("mUpSpeed: %f\n", mUpSpeed);
			if (mOwner->mJumpState == Jump && mUpSpeed < 0)
			{
				mOwner->mJumpState = Fall;
			}
			pos += mOwner->GetUp() * mUpSpeed * deltaTime;
		}
		
		mOwner->SetPosition(pos);
	}
}
