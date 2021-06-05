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

	// UI界面栈
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
	void ProcessInput(); //处理键盘输入
	void UpdateGame();  //更新游戏状态
	void GenerateOutput();  //游戏画面输出
	void LoadData();  //加载创建Actor和场景
	void UnloadData();  //删除场景和Actor

	
	// 储存游戏中所有的Actor
	std::vector<class Actor*> mActors;
	std::vector<class UIScreen*> mUIStack;
	std::unordered_map<std::string, class Font*> mFonts;
	// Map for text localization
	std::unordered_map<std::string, std::string> mText;

	// 储存待定的Actor
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
	// 输入

	class InputSystem* mInputSystem;
	class GameMode* mGameMode;
	//Actor
	class FPSActor* mFPSActor; //player
	class ShootActor* mShootActor; //enemy
	class SpriteComponent* mCrosshair; //准星
	std::vector<class PlaneActor*> mPlanes; //地面

	class ZmqClient* mZmqClient; //与RL算法通信的ZMQ客户端
	bool mZmqInit = false;
	bool GameReset = true;
};
