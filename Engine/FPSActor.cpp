#include "FPSActor.h"
#include "MoveComponent.h"
#include "Renderer.h"
#include "Game.h"
#include "GameMode.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "BulletActor.h"
#include "BoxComponent.h"
#include "PlaneActor.h"
#include "InputSystem.h"
#include "PhysWorld.h"
#include "Radar.h"
#include "Blackboard.h"
#include <iostream>
#include "json.hpp"
#include <cstdlib>

using json = nlohmann::json;

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	mMoveComp = new MoveComponent(this);

	mCameraComp = new FPSCamera(this);

	mRadar = new Radar(this);

	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshs/Rifle.gpmesh"));
	SetPosition(Vector3(-500.0f, -500.0f, 120.5f));
	// 添加BoxComponent
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-25.0f, -25.0f, -220.0f),
		Vector3(25.0f, 25.0f, 200.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
	//把初始的信息存到黑板里
	SetBlackboardObs(GetGame()->GetBlackboard()->mObs);
}


void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();
	//判断是否在地面
	PhysWorld* phys = GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	Vector3 start = GetPosition();
	
	Vector3 dir = GetUp();
	Vector3 end = start - dir * 300.0f;
	LineSegment ls(start, end);
	if (!phys->SegmentCast(ls, info))
	{
		
		mJumpState = Jump;
	}

	
	// Update position of FPS model relative to actor position
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);
	// Initialize rotation to actor rotation
	Quaternion q = GetRotation();
	// Rotate by pitch from camera
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
	SetBlackboardObs(GetGame()->GetBlackboard()->mObs);
}

void FPSActor::ActorInput(const class InputState* state)
{
	if (GetGame()->GetGameMode()->GetPlayerMode() == Player)
	{
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
			mMoveComp->SetUpSpeed(400.0f);
		}


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
		std::cout<< angularSpeed<<std::endl;
		mMoveComp->SetAngularSpeed(angularSpeed);

		// Compute pitch
		const float maxPitchSpeed = Math::Pi * 8;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// Convert to ~[-1.0, 1.0]
			pitchSpeed = y / maxMouseSpeed;
			pitchSpeed *= maxPitchSpeed;
		}
		mCameraComp->SetPitchSpeed(pitchSpeed);
	}
	else
	{
		float forwardSpeed = 0.0f;
		float strafeSpeed = 0.0f;
		float angularSpeed = 0.0f;
		float pitchSpeed = 0.0f;
		RLAction action = GetGame()->GetBlackboard()->mObs["action"].get<RLAction>();
		std::vector<float> speed;
		switch (action)
		{
		case Move:
			speed = MoveTo();
			forwardSpeed = speed[0];
			angularSpeed = speed[1];
			//std::cout<< forwardSpeed<<std::endl;
			//std::cout << strafeSpeed << std::endl;
			break;
		case RLAttack:
			Shoot();
			break;
		}
		mMoveComp->SetForwardSpeed(forwardSpeed);
		mMoveComp->SetStrafeSpeed(strafeSpeed);
		mMoveComp->SetAngularSpeed(angularSpeed);
		mCameraComp->SetPitchSpeed(pitchSpeed);
	}
	
}

void FPSActor::Shoot()
{
	// Get start point (in center of screen on near plane)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get end point (in center of screen, between near and far)
	screenPoint.z = 0.9f;
	Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get direction vector
	Vector3 dir = end - start;
	dir.Normalize();
	
	//子弹
	BulletActor* ball = new BulletActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir*20.0f);
	// Rotate the ball to face new direction
	ball->RotateToNewForward(dir);
	 //Play shooting sound
	//mAudioComp->PlayEvent("event:/Shot");
}



std::vector<float> FPSActor::MoveTo()
{
	std::vector<float> speed;
	float forwardSpeed = 400.0f;
	float angularSpeed = 0.0f;
	if (mRadar->GetTargetEnemy())
	{
		
		Vector3 position = mRadar->GetTargetEnemy()->GetPosition();
		Vector3 mPos = GetPosition();
		position.z = mPos.z;
		Vector3 dir = position - mPos;
		dir = Vector3::Normalize(dir);
		RotateToNewForward(dir);
	}
	else
	{
		/*Vector3 OwnForward = GetForward();
		Quaternion q = Quaternion(Vector3::UnitZ, Math::Pi);
		Vector3 dir = Vector3::Transform(OwnForward, q);
		RotateToNewForward(dir);
		float forwardSpeed = 400.0f;*/
		int random = rand() % 10;
		if(random<8)
			angularSpeed = 1;
		else
			angularSpeed = -1;
	}
	speed.push_back(forwardSpeed);
	speed.push_back(angularSpeed);
	return speed;
}


void FPSActor::TakeDamage()
{
	Health -= 10;
	GetGame()->GetBlackboard()->mObs["reward"] = GetGame()->GetBlackboard()->mObs["reward"].get<float>() - 0.5;
	/*std::cout << "Health-" << std::endl;*/
	if (Math::NearZero(Health))
	{
		//std::cout<<"player dead"<<std::endl;
		GetGame()->GetBlackboard()->mObs["reward"] = GetGame()->GetBlackboard()->mObs["reward"].get<float>() - 5.0;
		mState = EDead;
		GetGame()->GetBlackboard()->mObs["done"] = 1;
	}
}

void FPSActor::SetBlackboardObs(json& obs)
{

	//判断是否看到了敌方目标
	if (mRadar->GetTargetEnemy())
	{
		/*obs["Player"]["TargetPos_x"] = mRadar->GetTargetEnemy()->GetPosition().x;
		obs["Player"]["TargetPos_y"] = mRadar->GetTargetEnemy()->GetPosition().y;
		obs["Player"]["TargetPos_z"] = mRadar->GetTargetEnemy()->GetPosition().z;*/
		Vector3 emeny_pos = mRadar->GetTargetEnemy()->GetPosition();
		Vector3 m_pos = GetPosition();
		emeny_pos.Normalize();
		m_pos.Normalize();
		float distance = Math::Sqrt(emeny_pos.x* emeny_pos.x+ emeny_pos.y * emeny_pos.y+ emeny_pos.z * emeny_pos.z);
		float angle = Math::Acos(Vector3::Dot(emeny_pos, m_pos));
		obs["S"]["distance"] = distance;
		obs["S"]["angle"] = angle;
		obs["reward"] = obs["reward"].get<float>() + 0.02f;
	}
	else
	{
		//obs["Player"]["TargetPos_x"] = "None";
		//obs["Player"]["TargetPos_y"] = "None";
		//obs["Player"]["TargetPos_z"] = "None";
		obs["S"]["distance"] = 1000.0f;
		obs["S"]["angle"] = 180.0f;
		obs["reward"] = obs["reward"].get<float>() - 0.02f;
	}
	
}


void FPSActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

void FPSActor::FixCollisions()
{
	// Need to recompute my world transform to update world box
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	//std::cout<<"mMin.x"<< playerBox.mMin.x<<std::endl;
	//std::cout << "mMin.y" << playerBox.mMin.y << std::endl;
	//std::cout << "mMin.z" << playerBox.mMin.z << std::endl;
	//std::cout << "mMax.x" << playerBox.mMax.x << std::endl;
	//std::cout << "mMax.y" << playerBox.mMax.y << std::endl;
	//std::cout << "mMax.z" << playerBox.mMax.z << std::endl;
	//std::cout << "pos.x" << pos.x << std::endl;
	//std::cout << "pos.y" << pos.y << std::endl;
	//std::cout << "pos.z" << pos.z << std::endl;

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
			}

			// Need to set position and update box component
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}
