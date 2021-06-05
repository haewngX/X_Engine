#include "StateMachine.h"



StateMachine::StateMachine(State* StateInit)
	:mInitialState(StateInit),
	 mCurrentState(StateInit)
{
}

void StateMachine::Update(const std::vector<float>& Event)
{

	if (mCurrentState->IsTriggered(Event))
	{
		State* TargetState = mCurrentState->GetTargetState(Event);
		mCurrentState->GetExitActions();
		TargetState->GetEntryActions();
		mCurrentState = TargetState;
	}
	else
	{
		mCurrentState->GetActions();
	}
}


State::State()
{
}


void State::GetActions()
{
}


void State::GetEntryActions()
{
}


void State::GetExitActions()
{
}


bool State::IsTriggered(const std::vector<float>& Event)
{
	return false;
}


State* State::GetTargetState(const std::vector<float>& Event)
{
	return 0;
}

