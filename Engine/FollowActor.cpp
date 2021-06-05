#include "FollowActor.h"
#include "SkeletalMeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "FollowCamera.h"
#include "MoveComponent.h"
#include "InputSystem.h"
#include "PhysWorld.h"
#include "BoxComponent.h"
#include "PlaneActor.h"
#include "GunActor.h"
#include <iostream>

FollowActor::FollowActor(Game* game)
	:Actor(game)
	,mMoving(false)
{
	mMeshComp = new SkeletalMeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshs/SK_Mannequin.gpmesh"));
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/Meshs/SK_Mannequin.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
	SetPosition(Vector3(0.0f, 0.0f, -87.5f));

	//mGun = new GunActor(game, this);


	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();

	// 添加BoxComponent
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-25.0f, -25.0f, -87.5f),
		Vector3(25.0f, 25.0f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

FollowActor::~FollowActor()
{
	//delete mGun;
}

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();
	//判断是否在地面
	PhysWorld* phys = GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	Vector3 start = GetPosition();
	Vector3 dir = GetUp();
	Vector3 end = start - dir * 87.5f;
	LineSegment ls(start, end);
	if (!phys->SegmentCast(ls, info))
	{
		mJumpState = Jump;
	}

}

void FollowActor::ActorInput(const class InputState* state)
{
	std::map<int, float> Event;
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	// wasd movement
	if (state->Keyboard.GetKeyValue(0))
	{
		forwardSpeed += 400.0f;
	}
	if (state->Keyboard.GetKeyValue(1))
	{
		forwardSpeed -= 400.0f;
	}
	if (state->Keyboard.GetKeyValue(2))
	{
		strafeSpeed -= 400.0f;
	}
	if (state->Keyboard.GetKeyValue(3))
	{
		strafeSpeed += 400.0f;
	}
	if (state->Keyboard.GetKeyValue(4) && mJumpState == OnGround)
	{
		mJumpState = Jump;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Jump_From_Stand_Ironsights.gpanim"));
		mMoveComp->SetUpSpeed(400.0f);
	}
	
	Event[Forward] = forwardSpeed;
	Event[Right] = strafeSpeed;
	PlayActionAnimations(Event);

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);

	// Mouse movement
	// Get relative movement from SDL
	float x, y;
	state->Mouse.GetRelativeMouseState(&x, &y);
	// Assume mouse movement is usually between -500 and +500
	const int maxMouseSpeed = 50;
	// Rotation/sec at maximum speed
	const float maxAngularSpeed = Math::Pi * 8;
	float angularSpeed = 0.0f;
	if (x != 0)
	{
		// Convert to ~[-1.0, 1.0]
		angularSpeed = x / maxMouseSpeed;
		// Multiply by rotation/sec
		angularSpeed *= maxAngularSpeed;
	}
	mMoveComp->SetAngularSpeed(angularSpeed);


	// Compute pitch
	const float maxPitchSpeed = Math::Pi * 8;
	float pitchSpeed = 0.0f;
	if (y != 0)
	{
		// Convert to ~[-1.0, 1.0]
		pitchSpeed = y / (maxMouseSpeed *0.001);
		pitchSpeed *= maxPitchSpeed;
	}
	mCameraComp->SetPitchSpeed(pitchSpeed);

	if (state->Mouse.GetButtonValue(0))
	{
		Attack();
	}
}

void FollowActor::FixCollisions()
{
	// Need to recompute my world transform to update world box
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		// Do we collide with this PlaneActor?
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			// Calculate all our differences
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			// Set dx to whichever of dx1/dx2 have a lower abs
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ?
				dx1 : dx2;
			// Ditto for dy
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ?
				dy1 : dy2;
			// Ditto for dz
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ?
				dz1 : dz2;

			// Whichever is closest, adjust x/y position
			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;

			}
			else
			{
				pos.z += dz;
				mJumpState = OnGround;
				mMoveComp->SetUpSpeed(0.0f);
			}

			// Need to set position and update box component
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}

void FollowActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

void FollowActor::PlayActionAnimations(std::map<int, float>& Event)
{
	/*if (mJumpState == Fall)
	{
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/ALS_N_Land.gpanim"));
	}*/


	if ((mActionState != Forward) && (Event[Forward] > 0) && Math::NearZero(Event[Right]))
	{
		mActionState = Forward;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkFwd.gpanim"));
		
	}
	if ((mActionState != ForwardRight) && (Event[Forward] > 0) && (Event[Right] > 0))
	{
		mActionState = ForwardRight;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkRight.gpanim"));
	}
	else if ((mActionState != ForwardLeft) && (Event[Forward] > 0) && (Event[Right] < 0))
	{
		mActionState = ForwardLeft;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkLeft.gpanim"));
	}
	else if ((mActionState != Back) && (Event[Forward] < 0) && Math::NearZero(Event[Right]))
	{
		mActionState = Back;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkBwd.gpanim"));
	}
	else if ((mActionState != BackLeft) && (Event[Forward] < 0) && (Event[Right] < 0))
	{
		mActionState = BackLeft;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkLeft.gpanim"));
	}
	else if ((mActionState != BackRight) && (Event[Forward] < 0) && (Event[Right] > 0))
	{
		mActionState = BackRight;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkRight.gpanim"));
	}
	else if ((mActionState != Left) && Math::NearZero(Event[Forward]) && (Event[Right] < 0))
	{
		mActionState = Left;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkLeft.gpanim"));
	}
	else if ((mActionState != Right) && Math::NearZero(Event[Forward]) && (Event[Right] > 0))
	{
		mActionState = Right;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/AI_WalkRight.gpanim"));
	}


	if (Math::NearZero(Event[Forward]) && Math::NearZero(Event[Right]) && Math::NearZero(mMoveComp->GetUpSpeed()))
	{
		mActionState = Idle;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
	}
}

void FollowActor::Attack()
{
	
}
