#pragma once
#include "Actor.h"

class BulletActor : public Actor
{
public:
	BulletActor(class Game* game);

	void UpdateActor(float deltaTime) override;
	void SetPlayer(Actor* player);

	void HitTarget();
private:
	class AudioComponent* mAudioComp;
	class BulletMove* mMyMove;
	float mLifeSpan;
};
