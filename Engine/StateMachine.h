#pragma once
#include <vector>

class State
{
public:
	State();
	virtual void GetActions();
	virtual void GetEntryActions();
	virtual void GetExitActions();
	virtual bool IsTriggered(const std::vector<float>& Event);
	virtual State* GetTargetState(const std::vector<float>& Event);
};

class StateMachine
{
public:
	StateMachine(State* StateInit);

	void Update(const std::vector<float>& Event);
private:
	State* mInitialState;
	State* mCurrentState;

};



