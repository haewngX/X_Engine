#pragma once
#include <vector>
#include <functional>
#include "Math.h"
#include "Collision.h"

class PhysWorld
{
public:
	PhysWorld(class Game* game);

	struct CollisionInfo
	{
		// �ཻ���λ��
		Vector3 mPoint;
		// �ཻ��ķ���
		Vector3 mNormal;
		// ������ײ��Box
		class BoxComponent* mBox;
		// ������ײ��Actor
		class Actor* mActor;
	};

	// �ж��߶��Ƿ���BoxComponent�ཻ
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

	// Add/remove box components from world
	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);
private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
};