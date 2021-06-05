#include "Menu.h"
#include "Game.h"
#include "Renderer.h"
#include "DialogBox.h"
#include <SDL.h>
#include "GameMode.h"

PauseMenu::PauseMenu(Game* game)
	:UIScreen(game)
{
	mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("PauseTitle");
	AddButton("ResumeButton", [this]() {
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

PauseMenu::~PauseMenu()
{
	SetRelativeMouseMode(true);
	mGame->SetState(Game::EGameplay);
}

void PauseMenu::HandleKeyPress(const class InputState* state)
{
	UIScreen::HandleKeyPress(state);

	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Close();
	}
}



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

WinMenu::WinMenu(Game* game)
	:UIScreen(game)
{
	mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("WinTitle");
	AddButton("NewGameButton", [this]() {
		mGame->Reset();
		
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

WinMenu::~WinMenu()
{

}

void WinMenu::HandleKeyPress(const InputState* state)
{
	UIScreen::HandleKeyPress(state);
}

LoseMenu::LoseMenu(Game* game)
	:UIScreen(game)
{
	mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("LoseTitle");
	AddButton("NewGameButton", [this]() {
		mGame->Reset();

		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

LoseMenu::~LoseMenu()
{
}

void LoseMenu::HandleKeyPress(const InputState* state)
{
	UIScreen::HandleKeyPress(state);
}
