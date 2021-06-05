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
		// 相交点的位置
		Vector3 mPoint;
		// 相交点的法线
		Vector3 mNormal;
		// 发生碰撞的Box
		class BoxComponent* mBox;
		// 发生碰撞的Actor
		class Actor* mActor;
	};

	// 判断线段是否与BoxComponent相交
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