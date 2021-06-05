#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "Math.h"
#include "Game.h"
#include <vector>

//按键的状态
enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

//键盘的输入
class KeyboardState
{
public:
	KeyboardState(class Game* game);

	friend class InputSystem;

	// 是否获取keyCode的输入
	bool GetKeyValue(int keyCode) const;
	// 基于当前帧和之前获取状态
	ButtonState GetKeyState(int keyCode) const;
	int StateNum = 6;
private:
	class Game* mGame;
	bool mCurrState[6] = {0,0,0,0,0,0};
	bool mPrevState[6] = {0,0,0,0,0,0};
};

//鼠标的输入
class MouseState
{
public:
	MouseState(class Game* game);
	friend class InputSystem;

	// 鼠标的相对位置
	void GetPosition(GLFWwindow* window);
	void GetMousePosition(GLFWwindow* window, float* x, float* y) const;
	void GetRelativeMouseState(float* x, float* y) const;
	const Vector2& GetScrollWheel() const { return mScrollWheel; }
	bool IsRelative() const { return mIsRelative; }

	// 鼠标按键
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

	int StateNum = 3;
private:
	class Game* mGame;
	//储存当前鼠标的位置
	Vector2 mMousePos;
	Vector2 mMousePosPrev;
	Vector2 mMouseOffset;
	//鼠标滚轮移动
	Vector2 mScrollWheel;
	//储存按键信息
	bool mCurrButtons[3] = { 0,0,0 };
	bool mPrevButtons[3] = { 0,0,0 };
	//是否是relative mouse模式
	bool mIsRelative;
	//是否是第一次鼠标移动
	bool firstMouse = true;
};

class InputState

{
public:
	InputState(class Game* Game);
	KeyboardState Keyboard = KeyboardState(mGame);
	MouseState Mouse = MouseState(mGame);
private:
	class Game* mGame;
};

class InputSystem
{
public:
	InputSystem(class Game* Game);
	~InputSystem();

	// 在GLFW轮询前调用
	void PrepareForUpdate();
	// 在轮询之后调用，获取输入
	void Update();

	const InputState* GetState() const { return mState; }

private:
	class Game* mGame;
	class InputState* mState;
	float Filter1D(int input);
	Vector2 Filter2D(int inputX, int inputY);
	
};