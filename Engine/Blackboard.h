#pragma once
#include <vector>
#include "Math.h"
#include "json.hpp"

using json = nlohmann::json;

class Blackboard
{
public:
	Blackboard(class Game* game);

	void UpdateObs(class Actor* actor, json& obs);
	void WriteRLAction(json& action);
	json mObs;
	
private:
	class Game* mGame;
	

};