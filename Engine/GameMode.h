#pragma once
enum PlayerMode
{
	Player,
	RLAgent
};

enum Result
{
	Win,
	Lose
};
class GameMode
{
public:
	GameMode(class Game* game);
	void SetPlayerMode(PlayerMode pm) { mPlayerMode = pm;}
	PlayerMode GetPlayerMode() {return mPlayerMode;}
	void ActorKilled();
	void EndGame(enum Result r);
private:
	class Game* mGame;
	PlayerMode mPlayerMode = Player;
};