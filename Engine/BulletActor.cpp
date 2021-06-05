#include "BulletActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BulletMove.h"
#include <iostream>

BulletActor::BulletActor(Game* game)
	:Actor(game)
	,mLifeSpan(2.0f)
{
	//SetScale(10.0f);
	MeshComponent* mc = new MeshComponent(this);
	//Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshs/Sphere.gpmesh");
	//mc->SetMesh(mesh);
	mMyMove = new BulletMove(this);
	mMyMove->SetForwardSpeed(1500.0f);
}

void BulletActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
	
	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.0f)
	{
		SetState(EDead);
	}
}

void BulletActor::SetPlayer(Actor* player)
{
	mMyMove->SetPlayer(player);
}

void BulletActor::HitTarget()
{
	SetState(EDead);
	//mAudioComp->PlayEvent("event:/Ding");
}
