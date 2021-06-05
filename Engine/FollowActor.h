#pragma once
#include "Actor.h"
#include <map>


class FollowActor : public Actor
{
public:
	FollowActor(class Game* game);
	~FollowActor();
	void UpdateActor(float deltaTime) override;
	void ActorInput(const class InputState* state) override;
	class SkeletalMeshComponent* GetMeshComp() {return mMeshComp;}
	class MoveComponent* GetMoveComp() { return mMoveComp; }
	int GetActionState() { return mActionState; }
	void FixCollisions();
	void SetVisible(bool visible);
private:
	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class SkeletalMeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	//class GunActor* mGun;
	bool mMoving;
	int mActionState = Idle;
	void PlayActionAnimations(std::map<int, float>& Event);
	void Attack();
};
