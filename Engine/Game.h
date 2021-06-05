#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h> 

class Game
{
public:
	Game();
	~Game();
	bool Initialize(); 
	void RunLoop();  
	void Shutdown();  
	void Reset();  

	void AddActor(class Actor* actor);  
	void RemoveActor(class Actor* actor); 

	class Renderer* GetRenderer() { return mRenderer; } 
	class PhysWorld* GetPhysWorld() { return mPhysWorld; }
	class Blackboard* GetBlackboard() { return mBlackboard; }
	class InputSystem* GetInputSystem() { return mInputSystem; }
	class GameMode* GetGameMode() { return mGameMode; }
	class FPSActor* GetFPSActor() {return mFPSActor;}
	class ShootActor* GetShootActor() { return mShootActor; }

	// UI����ջ
	const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }
	void PushUI(class UIScreen* screen);

	enum GameState
	{
		EGameplay,
		EPaused,
		EQuit
	};
	GameState GetState() const { return mGameState; }
	void SetState(GameState state) { mGameState = state; }

	class Font* GetFont(const std::string& fileName);
	void LoadText(const std::string& fileName);
	const std::string& GetText(const std::string& key);

	class Skeleton* GetSkeleton(const std::string& fileName);
	class Animation* GetAnimation(const std::string& fileName);
	void AddPlane(class PlaneActor* plane);
	void RemovePlane(class PlaneActor* plane);
	std::vector<class PlaneActor*>& GetPlanes() { return mPlanes; }
private:
	void HandleKeyPress(const class InputState* state);
	void ProcessInput(); //�����������
	void UpdateGame();  //������Ϸ״̬
	void GenerateOutput();  //��Ϸ�������
	void LoadData();  //���ش���Actor�ͳ���
	void UnloadData();  //ɾ��������Actor

	
	// ������Ϸ�����е�Actor
	std::vector<class Actor*> mActors;
	std::vector<class UIScreen*> mUIStack;
	std::unordered_map<std::string, class Font*> mFonts;
	// Map for text localization
	std::unordered_map<std::string, std::string> mText;

	// ���������Actor
	// Map of loaded skeletons
	std::unordered_map<std::string, class Skeleton*> mSkeletons;
	// Map of loaded animations
	std::unordered_map<std::string, class Animation*> mAnims;
	std::vector<class Actor*> mPendingActors;

	class Renderer* mRenderer;  
	class PhysWorld* mPhysWorld; 
	class Blackboard* mBlackboard; 
	float lastFrame; 
	GameState mGameState;
	bool mUpdatingActors;
	// ����

	class InputSystem* mInputSystem;
	class GameMode* mGameMode;
	//Actor
	class FPSActor* mFPSActor; //player
	class ShootActor* mShootActor; //enemy
	class SpriteComponent* mCrosshair; //׼��
	std::vector<class PlaneActor*> mPlanes; //����

	class ZmqClient* mZmqClient; //��RL�㷨ͨ�ŵ�ZMQ�ͻ���
	bool mZmqInit = false;
	bool GameReset = true;
};
