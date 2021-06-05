#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "Math.h"
#include "Game.h"
#include <vector>

//������״̬
enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

//���̵�����
class KeyboardState
{
public:
	KeyboardState(class Game* game);

	friend class InputSystem;

	// �Ƿ��ȡkeyCode������
	bool GetKeyValue(int keyCode) const;
	// ���ڵ�ǰ֡��֮ǰ��ȡ״̬
	ButtonState GetKeyState(int keyCode) const;
	int StateNum = 6;
private:
	class Game* mGame;
	bool mCurrState[6] = {0,0,0,0,0,0};
	bool mPrevState[6] = {0,0,0,0,0,0};
};

//��������
class MouseState
{
public:
	MouseState(class Game* game);
	friend class InputSystem;

	// �������λ��
	void GetPosition(GLFWwindow* window);
	void GetMousePosition(GLFWwindow* window, float* x, float* y) const;
	void GetRelativeMouseState(float* x, float* y) const;
	const Vector2& GetScrollWheel() const { return mScrollWheel; }
	bool IsRelative() const { return mIsRelative; }

	// ��갴��
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

	int StateNum = 3;
private:
	class Game* mGame;
	//���浱ǰ����λ��
	Vector2 mMousePos;
	Vector2 mMousePosPrev;
	Vector2 mMouseOffset;
	//�������ƶ�
	Vector2 mScrollWheel;
	//���水����Ϣ
	bool mCurrButtons[3] = { 0,0,0 };
	bool mPrevButtons[3] = { 0,0,0 };
	//�Ƿ���relative mouseģʽ
	bool mIsRelative;
	//�Ƿ��ǵ�һ������ƶ�
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

	// ��GLFW��ѯǰ����
	void PrepareForUpdate();
	// ����ѯ֮����ã���ȡ����
	void Update();

	const InputState* GetState() const { return mState; }

private:
	class Game* mGame;
	class InputState* mState;
	float Filter1D(int input);
	Vector2 Filter2D(int inputX, int inputY);
	
};