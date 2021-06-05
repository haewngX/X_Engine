#pragma once
#include "Actor.h"

class GunActor : public Actor
{
public:


	GunActor(Game* game, class ShootActor* ShootCharacter);

	void UpdateActor(float deltaTime) override;
	//void ActorInput(const class InputState* state) override;

	//void Shoot();

	class Actor* GetGunModel() {return mGunModel;}
	void SetVisible(bool visible);
	void FixCollisions();
private:
	class MoveComponent* mMoveComp;
	class MeshComponent* mMeshComp;
	class Actor* mGunModel;
	class ShootActor* mShootCharacter;

};