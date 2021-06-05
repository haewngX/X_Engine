#pragma once
#include <vector>
#include <string>

enum Status
{
	Success,
	Fail,
	Running
};

class Node 
{
public:
	Node();
	void AddChild(Node* child);
	void SetParent(Node* parent);
	virtual Status Run(class AIPolicy* policy);
	std::vector<Node*> mChildren;

protected:
	Node* mParent;
	
	std::string mName = "";
};

class Sequence : public Node
{
public:
	Sequence();
	virtual Status Run(class AIPolicy* policy) override;

protected:
	std::string mName = "Sequence";
};

class Selector : public Node
{
public:
	Selector();
	virtual Status Run(class AIPolicy* policy) override;

protected:
	std::string mName = "Selector";
};

class Decorator : public Node
{
public:
	Decorator();

protected:
	std::string mName = "Decorator";
};

class ConditionEvaluator : public Decorator
{
public:
	ConditionEvaluator();
	virtual Status Run(class AIPolicy* policy) override;

protected:
	std::string mName = "ConditionEvaluator";
	
	virtual Status mRunFunc(class AIPolicy* policy);
};

class LeafNode : public Node
{
public:
	LeafNode();
	virtual Status Run(class AIPolicy* policy) override;

protected:

	virtual Status mRunFunc(class AIPolicy* policy);
};

class ActionNode : public LeafNode
{
public:
	ActionNode();

protected:
	virtual Status mRunFunc(class AIPolicy* policy);
};