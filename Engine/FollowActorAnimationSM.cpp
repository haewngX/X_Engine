#include "FollowActorAnimationSM.h"
#include "FollowActor.h"

Idle::Idle(FollowActor* actor, std::map<int, State*>& TargetStates)
:mTargetStates(TargetStates),
 mActor(actor)
{
}

void Idle::GetActions()
{
	mActor->GetMeshComp()->PlayAnimation(mActor->GetGame()->GetAnimation("Assets/Animations/idle01.gpanim"));
}

bool Idle::IsTriggered(std::map<int, float>& Event)
{
	if(Math::NearZero(Event[forwardspeed]) && Math::NearZero(Event[strafeSpeed]))
		return false;
	else
	{
		return true;
	}
}

State* Idle::GetTargetState(std::map<int, float>& Event)
{
	if ((Event[forwardspeed] > 0) && Math::NearZero(Event[strafeSpeed]))
	{

		return mTargetStates[Forward];
	}
	else if ((Event[forwardspeed] > 0) && Event[strafeSpeed] > 0)
	{
		return mTargetStates[ForwardRight];
	}
	else if ((Event[forwardspeed] > 0) && Event[strafeSpeed] > 0)
	{
		return mTargetStates[ForwardRight];
	}
}

