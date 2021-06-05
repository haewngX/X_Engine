#pragma once

#include "BehaviorTree.h"
#include "json.hpp"
using nlohmann::json;

class AIPolicy
{
public:
	AIPolicy(class Actor* actor);
	~AIPolicy();
	class Actor* GetActor() {return mActor;}
	void Action(json& obs);
	class Node* mCur;
	json mObs;
private:
	void CreateBehavior();
	void RecordNode(class Node* node);
	class Node* mRoot;
	class Actor* mActor;
	std::vector<class Node*> mNodes;
	
};

class Chase : public ActionNode
{
public:
	Chase();
	std::string mName = "Chase";
private:
	virtual Status mRunFunc(class AIPolicy* policy) override;
	
};

class Shoot : public ActionNode
{
public:
	Shoot();
	std::string mName = "Shoot";
private:
	virtual Status mRunFunc(class AIPolicy* policy) override;

};

class StayInPlace : public ActionNode
{
public:
	StayInPlace();
	std::string mName = "StayInPlace";
private:
	virtual Status mRunFunc(class AIPolicy* policy) override;

};


class CanSeePlayer : public ConditionEvaluator
{
public:
	CanSeePlayer();
	std::string mName = "CanSeePlayer";
private:
	virtual Status mRunFunc(class AIPolicy* policy) override;

};

class InWeaponRange : public ConditionEvaluator
{
public:
	InWeaponRange();
	std::string mName = "InWeaponRange";
private:
	virtual Status mRunFunc(class AIPolicy* policy) override;

};
