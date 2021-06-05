#include "GameMode.h"
#include "FPSActor.h"
#include "ShootActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Menu.h"
#include <iostream>

GameMode::GameMode(Game* game)
	:mGame(game)
{
}

void GameMode::ActorKilled()
{
	Actor::State ShootActorState = mGame->GetShootActor()->GetState();
	Actor::State PlayerState = mGame->GetFPSActor()->GetState();
	if (ShootActorState == Actor::EDead)
	{
		
		EndGame(Win);
	}
	if (PlayerState == Actor::EDead)
	{
		
		EndGame(Lose);
	}
}

void GameMode::EndGame(enum Result r)
{
	if (r == Win && mPlayerMode == Player)
	{
		new WinMenu(mGame);
		mGame->SetState(Game::EPaused);
		mGame->GetFPSActor()->GetFPSModel()->SetState(Actor::EDead);
		mGame->GetFPSActor()->SetState(Actor::EDead);
	}
	else if (r == Lose && mPlayerMode == Player)
	{
		new LoseMenu(mGame);
		mGame->SetState(Game::EPaused);
		mGame->GetFPSActor()->GetFPSModel()->SetState(Actor::EDead);
		mGame->GetShootActor()->SetState(Actor::EDead);
	}
	else if (r == Win && mPlayerMode == RLAgent)
	{
		mGame->SetState(Game::EPaused);
		mGame->GetFPSActor()->GetFPSModel()->SetState(Actor::EDead);
		mGame->GetFPSActor()->SetState(Actor::EDead);
	}
	else if (r == Lose && mPlayerMode == RLAgent)
	{
		mGame->SetState(Game::EPaused);
		mGame->GetFPSActor()->GetFPSModel()->SetState(Actor::EDead);
		mGame->GetShootActor()->SetState(Actor::EDead);
	}
	//glfwSetWindowShouldClose(mGame->GetRenderer()->GetWindow(), true);
	//mGame->SetState(Game::EQuit);
}
