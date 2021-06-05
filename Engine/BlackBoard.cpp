#include "Blackboard.h"
#include "Actor.h"

Blackboard::Blackboard(Game* game)
:mGame(game)
{

}

void Blackboard::UpdateObs(Actor* actor, json& obs)
{
	actor->SetBlackboardObs(obs);
}

void Blackboard::WriteRLAction(json& action)
{
	mObs["action"] = action["content"];

}
