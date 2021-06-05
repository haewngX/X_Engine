#pragma once
#include "Actor.h"

enum RLAction
{
	Move,
	RLAttack,
};

class FPSActor : public Actor
{
public:
	

	FPSActor(class Game* game);
	void UpdateActor(float deltaTime) override;
	void ActorInput(const class InputState* state) override;

	void Shoot();
	std::vector<float> MoveTo();

	void TakeDamage();
	void SetBlackboardObs(json& obs) override;
	class Actor* GetFPSModel() {return mFPSModel;}
	void SetVisible(bool visible);
	void FixCollisions();
private:
	class MoveComponent* mMoveComp;
	class MeshComponent* mMeshComp;
	class FPSCamera* mCameraComp;
	class BoxComponent* mBoxComp;
	class Actor* mFPSModel;
	class Radar* mRadar;
	float MaxHealth = 100;
	float Health = 100;

};