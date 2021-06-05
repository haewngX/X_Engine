#pragma once
#include "StateMachine.h"

#include <map>

enum ActorState
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
	OnGround,
	Jump,
	Fall
};

enum EventInput
{
	forwardspeed,
	strafeSpeed
};


class Idle : public State
{
public:
	Idle(class FollowActor* actor, std::map<int, State*>& TargetStates);
	virtual void GetActions();
	virtual bool IsTriggered(std::map<int, float>& Event);
	virtual State* GetTargetState(std::map<int, float>& Event);
private:
	class FollowActor* mActor;
	std::map<int, State*> mTargetStates;
};



