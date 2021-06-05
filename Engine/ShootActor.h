#pragma once
#include "Actor.h"

#include <map>



class ShootActor : public Actor
{
public:
	ShootActor(class Game* game);
	~ShootActor();
	void UpdateActor(float deltaTime) override;
	void ActorInput(const class InputState* state) override;
	class SkeletalMeshComponent* GetMeshComp() { return mMeshComp; }
	class MoveComponent* GetMoveComp() { return mMoveComp; }
	int GetActionState() { return mActionState; }
	class GunActor* GetGun() { return mGun; }
	void FixCollisions();
	void SetVisible(bool visible);
	void TakeDamage();
	void SetBlackboardObs(json& obs) override;
	//void Action(json& obs);
	void MoveTo(class Vector3 &position);
	void RotateTo(class Vector3& position);
	void Shoot(Vector3& position);
	int ShootTime = 0;
	int ShootInterval = 100;
	int GunRange = 1000;
	int mActionState = Idle;
	int DeadAnmiTime = 0;
private:
	class MoveComponent* mMoveComp;
	class SkeletalMeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	class GunActor* mGun;
	class Radar* mRadar;
	class AIPolicy* mAIpolicy;
	bool mMoving;
	
	float MaxHealth = 100;
	float Health = 100;
	
	
	void PlayActionAnimations(std::map<int, float>& Event);
	
	void Dead();
	
	
};