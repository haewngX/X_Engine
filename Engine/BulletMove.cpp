#include "BulletMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "ShootActor.h"
#include "FPSActor.h"
#include "BulletActor.h"


BulletMove::BulletMove(Actor* owner)
	:MoveComponent(owner)
{
}

void BulletMove::Update(float deltaTime)
{
	// Construct segment in direction of travel
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;

	// Create line segment
	LineSegment l(start, end);

	// Test segment vs world
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// (Don't collide vs player)
	if (phys->SegmentCast(l, info) && info.mActor != mPlayer)
	{
		//// If we collided, reflect the ball about the normal
		//dir = Vector3::Reflect(dir, info.mNormal);
		//mOwner->RotateToNewForward(dir);
		// Did we hit a target?
		ShootActor* AItarget = dynamic_cast<ShootActor*>(info.mActor);
		if (AItarget)
		{
			static_cast<BulletActor*>(mOwner)->HitTarget();
			AItarget->TakeDamage();
		}
		FPSActor* Playertarget = dynamic_cast<FPSActor*>(info.mActor);
		if (Playertarget)
		{
			static_cast<BulletActor*>(mOwner)->HitTarget();
			Playertarget->TakeDamage();
		}
	}

	// Base class update moves based on forward speed
	MoveComponent::Update(deltaTime);
}
