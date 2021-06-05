#include "Game.h"
#include <algorithm>
#include "Renderer.h"
#include "PhysWorld.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "FPSActor.h"
#include "GunActor.h"
#include "PlaneActor.h"
#include "TargetActor.h"
#include "ShootActor.h"
#include "BulletActor.h"
#include <iostream>
#include <rapidjson/document.h>
#include "InputSystem.h"
#include "Skeleton.h"
#include "Animation.h"
#include "Blackboard.h"
#include "GameMode.h"
#include "UIScreen.h"
#include "Font.h"
#include "Menu.h"
#include "Zmq.h"
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <SDL_ttf.h>

Game::Game()
:mRenderer(nullptr)
,mPhysWorld(nullptr)
,mGameState(EGameplay)
,mUpdatingActors(false)
,mInputSystem(new InputSystem(this))
,mBlackboard(new Blackboard(this))
,mGameMode(new GameMode(this))
{
}

Game::~Game()
{
	delete mInputSystem;
	delete mBlackboard;
	delete mGameMode;
}

bool Game::Initialize()
{

	mBlackboard->mObs["reward"] = 0.0f;
	mBlackboard->mObs["done"] = 0;
	// ����Renderer
	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		printf("Failed to initialize renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}
	lastFrame = glfwGetTime(); //��ȡ��һ֡��ʱ��
	mPhysWorld = new PhysWorld(this);

	// ��ʼ�� SDL_ttf
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}
	LoadData(); 
	
	return true;
}

void Game::RunLoop()
{
	mBlackboard->mObs["reward"] = 0.0f;
	mBlackboard->mObs["done"] = 0;
	while (mGameState != EQuit)
	{
		if ((GameReset||!mZmqInit) && mGameMode->GetPlayerMode() == RLAgent)
		{
			
			if (!mZmqInit)
			{
				mZmqClient = new ZmqClient();
				mZmqClient->SendMsg(Start, "Train start");
			}
				
			if (mZmqInit)
			{
				mZmqClient->SendMsg(Start, "Train start");
				Reset();
			}
			mZmqInit = true;
			GameReset = false;
			
		}
		if (!GameReset&&mZmqInit && mGameMode->GetPlayerMode() == RLAgent)
		{

			zmq::message_t msg_rec;
			json recv_msg = mZmqClient->RecvMsg(msg_rec);
			if (recv_msg["type"] == Start)
			{
				recv_msg["content"] = 0;
				mBlackboard->WriteRLAction(recv_msg);
			}
			else if (recv_msg["type"] == Action_REP)
			{
				mBlackboard->WriteRLAction(recv_msg);
			}
			else if (recv_msg["type"] == SingleGameOver)
			{
				mShootActor->SetState(Actor::EDead);
				GameReset = true;
			}

		}
		ProcessInput();  //��������
		UpdateGame();  //������Ϸ
		GenerateOutput();  //Render���
		if (!GameReset && mZmqInit && mGameMode->GetPlayerMode() == RLAgent)
		{
			std::string str = mBlackboard->mObs.dump();
			mZmqClient->SendMsg(Obs_REQ, str);

		}
	}
}

void Game::AddPlane(PlaneActor* plane)
{
	mPlanes.emplace_back(plane);
}

void Game::RemovePlane(PlaneActor* plane)
{
	auto iter = std::find(mPlanes.begin(), mPlanes.end(), plane);
	mPlanes.erase(iter);
}

void Game::HandleKeyPress(const class InputState* state)
{

	//�������Esc����ʾ��ͣ�˵�
	if (glfwGetKey(mRenderer->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		new PauseMenu(this);
		/*glfwSetWindowShouldClose(mRenderer->GetWindow(), true);
		mGameState = EQuit;*/
	}

	if (state->Mouse.GetButtonState(0)== EReleased)
	{
		mFPSActor->Shoot();
	}
}

void Game::ProcessInput()

{
	mInputSystem->PrepareForUpdate();
	glfwPollEvents(); //��ȡGLFW��ѯ�¼�
	mInputSystem->Update();
	const InputState* state = mInputSystem->GetState();

	if (mGameState == EGameplay)
	{
		HandleKeyPress(state);
	}
	else if (!mUIStack.empty())
	{
		mUIStack.back()->HandleKeyPress(state);
	}
	////�������Esc���˳�
	//if (glfwGetKey(mRenderer->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//{
	//	glfwSetWindowShouldClose(mRenderer->GetWindow(), true);
	//	mGameState = EQuit;
	//}

	//HandleKeyPress(state);//�����ӵ�
	//����Actor��������
	if (mGameState == EGameplay)
	{
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EActive)
			{
				actor->ProcessInput(state);
			}
		}
	}
	else if (!mUIStack.empty())
	{
		mUIStack.back()->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	 //����delta time
	float currentFrame = glfwGetTime();
	float deltaTime = (currentFrame - lastFrame);
	//if (deltaTime > 0.05f)
	//{
	//	deltaTime = 0.05f;
	//}
	lastFrame = glfwGetTime();
	
	if (mGameState == EGameplay)
	{
		//�������е�Actors
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;

		// ��pending actors����mActors
		for (auto pending : mPendingActors)
		{
			pending->ComputeWorldTransform();
			mActors.emplace_back(pending);
		}
		mPendingActors.clear();

		mGameMode->ActorKilled();
			
		// ��dead actors����temp vector
		std::vector<Actor*> deadActors;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}

		// ɾ��dead actors
		for (auto actor : deadActors)
		{
			delete actor;
		}
	}
	// ���� UI 
	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::EActive)
		{
			ui->Update(deltaTime);
		}
	}
	// ɾ�� UIScreens 
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::EClosing)
		{
			delete* iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	// ����Ӣ��text
	LoadText("Assets/English.gptext");

	new InitMenu(this);

	Actor* a = nullptr;
	Quaternion q;
	//MeshComponent* mc = nullptr;

	//���õ���
	const float start = -550.0f;
	const float size = 500.0f;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			a = new PlaneActor(this);
			a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}


	//// ���ҵ�ǽ
	//q = Quaternion(Vector3::UnitX, Math::PiOver2);
	//for (int i = 0; i < 10; i++)
	//{
	//	a = new PlaneActor(this);
	//	a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
	//	a->SetRotation(q);
	//	
	//	a = new PlaneActor(this);
	//	a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
	//	a->SetRotation(q);
	//}

	//q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	//// ǰ���ǽ
	//for (int i = 0; i < 10; i++)
	//{
	//	a = new PlaneActor(this);
	//	a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
	//	a->SetRotation(q);

	//	a = new PlaneActor(this);
	//	a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
	//	a->SetRotation(q);
	//}

	// �ƹ�
	mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
	DirectionalLight& dir = mRenderer->GetDirectionalLight();
	dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
	dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
	dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

	a = new Actor(this);
	a->SetScale(2.0f);
	mCrosshair = new SpriteComponent(a);
	mCrosshair->SetTexture(mRenderer->GetTexture("Assets/Textures/Crosshair.png"));

	// FPSActor
	mShootActor = new ShootActor(this);
	mFPSActor = new FPSActor(this);
	
	
	//// Create target actors
	//a = new TargetActor(this);
	//a->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
	//a = new TargetActor(this);
	//a->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
	//a = new TargetActor(this);
	//a->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
	//a = new TargetActor(this);
	//a->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));
}

void Game::UnloadData()
{
	// ɾ��Actor
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	if (mRenderer)
	{
		mRenderer->UnloadData();
	}
	// Unload ����
	for (auto s : mSkeletons)
	{
		delete s.second;
	}

	// Unload����
	for (auto a : mAnims)
	{
		delete a.second;
	}
	// ��� UI 
	while (!mUIStack.empty())
	{
		delete mUIStack.back();
		mUIStack.pop_back();
	}
}

void Game::Shutdown()
{
	UnloadData();
	TTF_Quit();
	delete mPhysWorld;
	if (mRenderer)
	{
		mRenderer->Shutdown();
	}
}

void Game::Reset()
{
	mShootActor = new ShootActor(this);
	mFPSActor = new FPSActor(this);
	SetState(Game::EGameplay);
}

void Game::AddActor(Actor* actor)
{
	//����actor
	// ������ڸ���actors, ��Ҫ�ȼ��뵽pending��
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// �ж�Ҫ�Ƴ���actor�Ƿ���pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
	//����ǣ���mPendingActors���Ƴ�
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// �ж�Ҫ�Ƴ���actor�Ƿ���actors��?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		//����ǣ��Ƴ�
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}
void Game::PushUI(UIScreen* screen)
{
	mUIStack.emplace_back(screen);
}

Font* Game::GetFont(const std::string& fileName)
{
	auto iter = mFonts.find(fileName);
	if (iter != mFonts.end())
	{
		return iter->second;
	}
	else
	{
		Font* font = new Font(this);
		if (font->Load(fileName))
		{
			mFonts.emplace(fileName, font);
		}
		else
		{
			font->Unload();
			delete font;
			font = nullptr;
		}
		return font;
	}
}
void Game::LoadText(const std::string& fileName)
{
	// Clear the existing map, if already loaded
	mText.clear();
	// Try to open the file
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Text file %s not found", fileName.c_str());
		return;
	}
	// Read the entire file to a string stream
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	// Open this file in rapidJSON
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);
	if (!doc.IsObject())
	{
		SDL_Log("Text file %s is not valid JSON", fileName.c_str());
		return;
	}
	// Parse the text map
	const rapidjson::Value& actions = doc["TextMap"];
	for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
		itr != actions.MemberEnd(); ++itr)
	{
		if (itr->name.IsString() && itr->value.IsString())
		{
			mText.emplace(itr->name.GetString(),
				itr->value.GetString());
		}
	}
}
const std::string& Game::GetText(const std::string& key)
{
	static std::string errorMsg("**KEY NOT FOUND**");
	// Find this text in the map, if it exists
	auto iter = mText.find(key);
	if (iter != mText.end())
	{
		return iter->second;
	}
	else
	{
		return errorMsg;
	}
}
Skeleton* Game::GetSkeleton(const std::string& fileName)
{
	auto iter = mSkeletons.find(fileName);
	if (iter != mSkeletons.end())
	{
		return iter->second;
	}
	else
	{
		Skeleton* sk = new Skeleton();
		if (sk->Load(fileName))
		{
			mSkeletons.emplace(fileName, sk);
		}
		else
		{
			delete sk;
			sk = nullptr;
		}
		return sk;
	}
}

Animation* Game::GetAnimation(const std::string& fileName)
{
	auto iter = mAnims.find(fileName);
	if (iter != mAnims.end())
	{
		return iter->second;
	}
	else
	{
		Animation* anim = new Animation();
		if (anim->Load(fileName))
		{
			mAnims.emplace(fileName, anim);
		}
		else
		{
			delete anim;
			anim = nullptr;
		}
		return anim;
	}
}