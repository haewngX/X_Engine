#include "ShootActor.h"
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
#include "Radar.h"
#include "Blackboard.h"
#include "AIPolicy.h"
#include "BulletActor.h"
#include <iostream>

ShootActor::ShootActor(Game* game)
	:Actor(game)
	, mMoving(false)
	
{
	mMeshComp = new SkeletalMeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshs/SK_Mannequin.gpmesh"));
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/Meshs/SK_Mannequin.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
	SetPosition(Vector3(500.0f, 500.0f, -100.0f));
	Quaternion q = Quaternion(Vector3::UnitZ, Math::Pi);
	SetRotation(q);

	mGun = new GunActor(game, this);
	mRadar = new Radar(this);
	mAIpolicy = new AIPolicy(this);

	mMoveComp = new MoveComponent(this);

	// 添加BoxComponent
	mBoxComp = new BoxComponent(this);
	//AABB myBox(Vector3(-525.0f, -525.0f, -50.5f),
	//	Vector3(525.0f, 525.0f, 100.5f));

	AABB myBox(Vector3(-25.0f, -25.0f, -50.0f),
		Vector3(25.0f, 25.0f, 200.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
	SetBlackboardObs(GetGame()->GetBlackboard()->mObs);
}

ShootActor::~ShootActor()
{
	delete mAIpolicy;
}


void ShootActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();
	//判断是否在地面
	PhysWorld* phys = GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	Vector3 start = GetPosition();
	Vector3 dir = GetUp();
	Vector3 end = start - dir * 100.0f;
	LineSegment ls(start, end);
	if (!phys->SegmentCast(ls, info))
	{
		mJumpState = Jump;
	}

	//判断是否看到了敌方目标
	if (mRadar->GetTargetEnemy())
	{
		
	}
	if (DeadAnmiTime >0)
	{
		DeadAnmiTime++;
		if (DeadAnmiTime == 32)
		{
			mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
			mState = EDead;
			
		}
			
	}
	SetBlackboardObs(GetGame()->GetBlackboard()->mObs);

}

void ShootActor::ActorInput(const class InputState* state)
{
	mAIpolicy->Action(GetGame()->GetBlackboard()->mObs);
	//std::map<int, float> Event;
	//float forwardSpeed = 0.0f;
	//float strafeSpeed = 0.0f;
	//// wasd movement
	//if (state->Keyboard.GetKeyValue(0))
	//{
	//	forwardSpeed += 400.0f;
	//}
	//if (state->Keyboard.GetKeyValue(1))
	//{
	//	forwardSpeed -= 400.0f;
	//}
	//if (state->Keyboard.GetKeyValue(2))
	//{
	//	strafeSpeed -= 400.0f;
	//}
	//if (state->Keyboard.GetKeyValue(3))
	//{
	//	strafeSpeed += 400.0f;
	//}
	//if (state->Keyboard.GetKeyValue(4) && mJumpState == OnGround)
	//{
	//	mJumpState = Jump;
	//	mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Jump_From_Stand_Ironsights.gpanim"));
	//	mMoveComp->SetUpSpeed(400.0f);
	//}

	//Event[Forward] = forwardSpeed;
	//Event[Right] = strafeSpeed;
	//PlayActionAnimations(Event);

	//mMoveComp->SetForwardSpeed(forwardSpeed);
	//mMoveComp->SetStrafeSpeed(strafeSpeed);

	//// Mouse movement
	//// Get relative movement from SDL
	//float x, y;
	//state->Mouse.GetRelativeMouseState(&x, &y);
	//// Assume mouse movement is usually between -500 and +500
	//const int maxMouseSpeed = 50;
	//// Rotation/sec at maximum speed
	//const float maxAngularSpeed = Math::Pi * 8;
	//float angularSpeed = 0.0f;
	//if (x != 0)
	//{
	//	// Convert to ~[-1.0, 1.0]
	//	angularSpeed = x / maxMouseSpeed;
	//	// Multiply by rotation/sec
	//	angularSpeed *= maxAngularSpeed;
	//}
	//mMoveComp->SetAngularSpeed(angularSpeed);


	//// Compute pitch
	//const float maxPitchSpeed = Math::Pi * 8;
	//float pitchSpeed = 0.0f;
	//if (y != 0)
	//{
	//	// Convert to ~[-1.0, 1.0]
	//	pitchSpeed = y / (maxMouseSpeed * 0.001);
	//	pitchSpeed *= maxPitchSpeed;
	//}

}


void ShootActor::FixCollisions()
{
	// 重新计算world transform to update world box
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

void ShootActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

void ShootActor::TakeDamage()
{
	Health -= 10;
	GetGame()->GetBlackboard()->mObs["reward"] = GetGame()->GetBlackboard()->mObs["reward"].get<float>()+0.5;

	if (Math::NearZero(Health))
	{
		Dead();	
		GetGame()->GetBlackboard()->mObs["reward"] = GetGame()->GetBlackboard()->mObs["reward"].get<float>() + 5.0;
		GetGame()->GetBlackboard()->mObs["done"] = 1;
	}
}

void ShootActor::SetBlackboardObs(json& obs)
{
	//判断是否看到了敌方目标
	if (mRadar->GetTargetEnemy())
	{
		obs["AI"]["TargetPos_x"] = mRadar->GetTargetEnemy()->GetPosition().x;
		obs["AI"]["TargetPos_y"] = mRadar->GetTargetEnemy()->GetPosition().y;
		obs["AI"]["TargetPos_z"] = mRadar->GetTargetEnemy()->GetPosition().z;
	}
	else
	{
		obs["AI"]["TargetPos_x"] = Math::Infinity;
		obs["AI"]["TargetPos_y"] = Math::Infinity;
		obs["AI"]["TargetPos_z"] = Math::Infinity;
	}
}

void ShootActor::PlayActionAnimations(std::map<int, float>& Event)
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
	else if ((mActionState != ForwardRight) && (Event[Forward] > 0) && (Event[Right] > 0))
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


	else if (Math::NearZero(Event[Forward]) && Math::NearZero(Event[Right]) && Math::NearZero(mMoveComp->GetUpSpeed()))
	{
		mActionState = Idle;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
	}
}

void ShootActor::MoveTo(Vector3& position)
{
	std::map<int, float> Event;
	Vector3 mPos = GetPosition();
	position.z = mPos.z;
	Vector3 dir = position - mPos;
	dir = Vector3::Normalize(dir);
	RotateToNewForward(dir);
	float forwardSpeed = 200.0f;
	Event[Forward] = forwardSpeed;
	PlayActionAnimations(Event);
	mMoveComp->SetForwardSpeed(forwardSpeed);
}

void ShootActor::RotateTo(Vector3& position)
{
	Vector3 start = GetPosition();
	position.z = start.z;
	// Get end point (in center of screen, between near and far)
	//start.z = position.z;
	// Get direction vector
	Vector3 dir = position - start;
	dir.Normalize();
	RotateToNewForward(dir);
}

void ShootActor::Shoot(Vector3& position)
{
	ShootTime = 0;
	// Get start point (in center of screen on near plane) 
	Vector3 start = mGun->GetPosition();
	position.z = start.z;
	// Get end point (in center of screen, between near and far)
	//start.z = position.z;
	// Get direction vector
	Vector3 dir = position - start;
	dir.Normalize();
	//子弹
	BulletActor* ball = new BulletActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.0f);
	// Rotate the ball to face new direction
	ball->RotateToNewForward(dir);
	//Play shooting sound
   //mAudioComp->PlayEvent("event:/Shot");
}


void ShootActor::Dead()
{
	mGun->GetGunModel()->SetState(Actor::EDead);
	mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/Animations/Dead1.gpanim"));
	DeadAnmiTime++;
}
