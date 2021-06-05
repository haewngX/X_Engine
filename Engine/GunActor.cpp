#include "GunActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "ShootActor.h"
#include "MoveComponent.h"
#include <iostream>

GunActor::GunActor(Game* game, ShootActor* ShootCharacter)
	:Actor(game)
	,mShootCharacter(ShootCharacter)
{
	mGunModel = new Actor(game);
	mGunModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mGunModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshs/M4_Rifle_01.gpmesh"));
	//SetPosition(Vector3(0.0f, 0.0f, 300.0f));
	mMoveComp = new MoveComponent(this);
}

void GunActor::UpdateActor(float deltaTime)
{
	// Update position of FPS model relative to actor position
	if (mShootCharacter)
	{
		mGunModel->SetPosition(mShootCharacter->GetPosition());
		if (mShootCharacter->GetActionState() == Forward)
		{
			mGunModel->SetPosition(mShootCharacter->GetPosition() + GetUp() * (-10));
		}

		// Initialize rotation to actor rotation
		mGunModel->SetRotation(mShootCharacter->GetRotation());
	}
	else
	{
		mGunModel->mState = EDead;
		mState = EDead;
	}
	

}

void GunActor::SetVisible(bool visible)
{
}

void GunActor::FixCollisions()
{
}
