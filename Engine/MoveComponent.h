#pragma once
#include "Component.h"

class MoveComponent : public Component
{
public:
	// Lower update order to update first
	MoveComponent(class Actor* owner, int updateOrder = 10);
	void Update(float deltaTime) override;
	
	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	float GetStrafeSpeed() const { return mStrafeSpeed; }
	float GetUpSpeed() const { return mUpSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
	void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }
	void SetUpSpeed(float speed) { mUpSpeed = speed; }
protected:
	float mAngularSpeed;
	float mForwardSpeed;
	float mStrafeSpeed; //旋转速度
	float mUpSpeed; //跳跃速度
	float gravity = 980.0f; //重力模拟
};
