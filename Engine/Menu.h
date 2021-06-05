#pragma once
#include "UIScreen.h"

class PauseMenu : public UIScreen
{
public:
	PauseMenu(class Game* game);
	~PauseMenu();

	void HandleKeyPress(const class InputState* state) override;
};

class InitMenu : public UIScreen
{
public:
	InitMenu(class Game* game);
	~InitMenu();

	void HandleKeyPress(const class InputState* state) override;
};

class WinMenu : public UIScreen
{
public:
	WinMenu(class Game* game);
	~WinMenu();

	void HandleKeyPress(const class InputState* state) override;
};

class LoseMenu : public UIScreen
{
public:
	LoseMenu(class Game* game);
	~LoseMenu();

	void HandleKeyPress(const class InputState* state) override;
};