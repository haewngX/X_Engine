#pragma once
#include <vector>
#include <string>
#include "Math.h"
#include <cstdint>
#include "json.hpp"
using nlohmann::json;

enum JumpState
{
	OnGround,
	Jump,
	Fall
};
enum ActionState
{
	Idle,
	Forward,
	Back,
	Left,
	Right,
	ForwardLeft,
	ForwardRight,
	BackLeft,
	BackRight,
};

enum State
{
	EActive,
	EPaused,
	EDead
};



class Actor
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
	int mJumpState = OnGround;


	Actor(class Game* game);
	virtual ~Actor();

	// 更新
	void Update(float deltaTime);
	// 更新actor的components
	void UpdateComponents(float deltaTime);
	// 更新Actor
	virtual void UpdateActor(float deltaTime);

	void ProcessInput(const class InputState* state);
	virtual void ActorInput(const class InputState* state);

	virtual void SetBlackboardObs(json& obs);

	// Getters/setters
	const Vector3& GetPosition() const { return mPosition; }
	void SetPosition(const Vector3& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale;  mRecomputeWorldTransform = true; }
	const Quaternion& GetRotation() const { return mRotation; }
	void SetRotation(const Quaternion& rotation) { mRotation = rotation;  mRecomputeWorldTransform = true; }
	
	void ComputeWorldTransform(); //计算世界变换
	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	void RotateToNewForward(const Vector3& forward);
	Vector3 GetForward() const { return Vector3::Transform(Vector3::UnitX, mRotation); }
	Vector3 GetRight() const { return Vector3::Transform(Vector3::UnitY, mRotation); }
	Vector3 GetUp() const { return Vector3::Transform(Vector3::UnitZ, mRotation); }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }

	// Actor的状态
	State mState;
	// Add/remove components
	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);
private:
	

	//世界变换
	Matrix4 mWorldTransform;
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
	bool mRecomputeWorldTransform;

	std::vector<class Component*> mComponents;
	class Game* mGame;
	
};
