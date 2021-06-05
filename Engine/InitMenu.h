#pragma once
#include "UIScreen.h"

class InitMenu : public UIScreen
{
public:
	InitMenu(class Game* game);
	~InitMenu();

	void HandleKeyPress(const class InputState* state) override;
};