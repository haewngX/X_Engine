#include "InitMenu.h"
#include "Game.h"
#include "GameMode.h"
#include "Renderer.h"
#include "DialogBox.h"
#include <SDL.h>

InitMenu::InitMenu(Game* game)
	:UIScreen(game)
{
	mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("InitTitle");
	AddButton("PlayerButton", [this]() {
		mGame->GetGameMode()->SetPlayerMode(Player);
		Close();
	});
	AddButton("RLAgentButton", [this]() {
		
		mGame->GetGameMode()->SetPlayerMode(RLAgent);
		Close();
	});
}

InitMenu::~InitMenu()
{
	SetRelativeMouseMode(true);
	mGame->SetState(Game::EGameplay);
}

void InitMenu::HandleKeyPress(const class InputState* state)
{
	UIScreen::HandleKeyPress(state);

}