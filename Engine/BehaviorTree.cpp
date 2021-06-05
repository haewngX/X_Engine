#include "BehaviorTree.h"
#include "AIPolicy.h"
#include <iostream>

Node::Node()
{
}

void Node::AddChild(Node* child)
{
	mChildren.push_back(child);
}

Status Node::Run(AIPolicy* policy)
{
	return Success;
}



void Node::SetParent(Node* parent)
{
	mParent = parent;
}

Sequence::Sequence()
{

}

Status Sequence::Run(AIPolicy* policy)
{
	policy->mCur = this;
	for (auto child : mChildren)
	{
		Status status = child->Run(policy);
		if (status != Success)
		{
			return status;
		}
	}
	return Success;
}

Selector::Selector()
{
}

Status Selector::Run(AIPolicy* policy)
{
	//std::cout << mName << std::endl;
	policy->mCur = this;
	Status result = Fail;
	for (auto child : mChildren)
	{
		Status status = child->Run(policy);
		if (status != Fail)
		{
			result = status;
			break;
		}
	}
	return result;
}

Decorator::Decorator()
{
}

ConditionEvaluator::ConditionEvaluator()
{
}

Status ConditionEvaluator::Run(AIPolicy* policy)
{
	policy->mCur = this;
	Status status = mRunFunc(policy);
	if (status == Success)
	{
		status = mChildren[0]->Run(policy);
	}
	return status;
}

Status ConditionEvaluator::mRunFunc(AIPolicy* policy)
{
	return Success;
}

LeafNode::LeafNode()
{
}

Status LeafNode::Run(AIPolicy* policy)
{
	policy->mCur = this;
	Status status = Success;
	status = mRunFunc(policy);
	return status;
}

Status LeafNode::mRunFunc(AIPolicy* policy)
{
	return Success;
}

ActionNode::ActionNode()
{
}

Status ActionNode::mRunFunc(AIPolicy* policy)
{
	return Status();
}
