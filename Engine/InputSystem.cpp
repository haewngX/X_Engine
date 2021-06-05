#include "InputSystem.h"
#include "Game.h"
#include "Renderer.h"
#include <iostream>
KeyboardState::KeyboardState(Game* game)
	:mGame(game)
{
}

bool KeyboardState::GetKeyValue(int keyCode) const
{
	if (mCurrState[keyCode] == 1)
		return true;
	return false;
}


ButtonState KeyboardState::GetKeyState(int keyCode) const
{
	//如果之前该键没有被按下，现在也没有被按下，为None,如果现在被按下，则为Pressed
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	//如果之前该键被按下，现在没有被按下，为Released,如果现在被按下，则为Held
	else // Prev state must be 1
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

MouseState::MouseState(Game* game)
	:mGame(game)
{
}

void MouseState::GetPosition(GLFWwindow * window)
{
	double cur_x, cur_y;
	glfwGetCursorPos(window, &cur_x, &cur_y);
	if (firstMouse)
	{
		mMousePosPrev.x = cur_x;
		mMousePosPrev.y = cur_y;
		firstMouse = false;
	}

	mMousePos.x = static_cast<float>(cur_x);
	mMousePos.y = static_cast<float>(cur_y);

	mMouseOffset.x = mMousePos.x - mMousePosPrev.x;
	mMouseOffset.y = mMousePos.y - mMousePosPrev.y;

	mMousePosPrev.x = mMousePos.x;
	mMousePosPrev.y = mMousePos.y;
}

void MouseState::GetMousePosition(GLFWwindow* window, float* x, float* y) const
{
	double cur_x, cur_y;
	glfwGetCursorPos(window, &cur_x, &cur_y);
	*x = static_cast<float>(cur_x);
	*y = static_cast<float>(cur_y);
}

void MouseState::GetRelativeMouseState(float* x, float* y) const
{
	*x = mMouseOffset.x;
	*y = mMouseOffset.y;
}

bool MouseState::GetButtonValue(int button) const
{
	if (mCurrButtons[button] == 1)
		return true;
	return false;
}

ButtonState MouseState::GetButtonState(int button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

InputSystem::InputSystem(Game* Game)
	:mGame(Game)
	,mState(new InputState(mGame))
{
	
}

InputSystem::~InputSystem()
{
	delete mState;
}

void InputSystem::PrepareForUpdate()
{
	//键盘
	memcpy(mState->Keyboard.mPrevState,
		mState->Keyboard.mCurrState,
		sizeof(mState->Keyboard.mCurrState));
	memset(mState->Keyboard.mCurrState, 0,
		sizeof(mState->Keyboard.mCurrState));

	//鼠标
	memcpy(mState->Mouse.mPrevButtons,
		mState->Mouse.mCurrButtons,
		sizeof(mState->Mouse.mCurrButtons));
	memset(mState->Mouse.mCurrButtons, 0,
		sizeof(mState->Mouse.mCurrButtons));

}

void InputSystem::Update()
{
	//键盘
	
	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
	{
		mState->Keyboard.mCurrState[0] = 1;

	}
		
	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		mState->Keyboard.mCurrState[1] = 1;
		
	}
	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		mState->Keyboard.mCurrState[2] = 1;
	}
	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		mState->Keyboard.mCurrState[3] = 1;
	}
	if (glfwGetKey(mGame->GetRenderer()->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mState->Keyboard.mCurrState[4] = 1;
	}


	//鼠标
	//glfwSetInputMode(mGame->GetRenderer()->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mState->Mouse.GetPosition(mGame->GetRenderer()->GetWindow());
	if (glfwGetMouseButton(mGame->GetRenderer()->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		mState->Mouse.mCurrButtons[0] = 1;
	if (glfwGetMouseButton(mGame->GetRenderer()->GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		mState->Mouse.mCurrButtons[1] = 1;
	if (glfwGetMouseButton(mGame->GetRenderer()->GetWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		mState->Mouse.mCurrButtons[2] = 1;

	//std::cout <<"Prev: ";
	//for (int i = 0; i < 4; i++)
	//{
	//	std::cout << mState->Keyboard.mPrevState[i] << " ";
	//}
	//std::cout << std::endl;
	//std::cout << "Cur: ";
	//for (int i = 0; i < 4; i++)
	//{
	//	std::cout << mState->Keyboard.mCurrState[i]<<" ";
	//}
	//std::cout << std::endl;
	//std::cout <<"Prev: ";
	//for (int i = 0; i < 3; i++)
	//{
	//	std::cout << mState->Mouse.mPrevButtons[i] << " ";
	//}
	//std::cout << std::endl;
	//std::cout << "Cur: ";
	//for (int i = 0; i < 3; i++)
	//{
	//	std::cout << mState->Mouse.mCurrButtons[i]<<" ";
	//}
	//std::cout << std::endl;
	
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
			(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}



InputState::InputState(Game* Game)
	:mGame(Game)
{
}
