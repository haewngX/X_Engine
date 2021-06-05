#pragma once
#include "MoveComponent.h"

class BulletMove : public MoveComponent
{
public:
	BulletMove(class Actor* owner);

	void SetPlayer(Actor* player) { mPlayer = player; }
	void Update(float deltaTime) override;
protected:
	class Actor* mPlayer;
};