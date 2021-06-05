#include "Radar.h"
#include "Actor.h"
#include <iostream>
#include "Collision.h"
#include "PhysWorld.h"
#include "ShootActor.h"
#include "FPSActor.h"
#include "Game.h"
#include "Renderer.h"

Radar::Radar(class Actor* owner, int updateOrder, float RadarRange, float RadarRadius)
	:Component(owner, updateOrder)
{
	mOwner = owner;
	mRadarRadius = RadarRadius;
	mRadarRange = RadarRange;
}

void Radar::Update(float deltaTime)
{
	mTargetEnemy = nullptr;
	Vector3 OwnPosition = mOwner->GetPosition();
	OwnPosition.z = 120;
	Vector3 OwnForward = mOwner->GetForward();

	for (float i = -mRadarRange;i< mRadarRange;i+=RangeStep)
	{
		i = Math::ToRadians(i);
		Quaternion q = Quaternion(Vector3::UnitZ, i);
		Vector3 dir = Vector3::Transform(OwnForward,q);
		
		Vector3 end = OwnPosition + dir * mRadarRadius;

		// 创建该方向的射线
		LineSegment l(OwnPosition + dir * 30, end);

		// 测试是否和敌方碰撞
		PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
		PhysWorld::CollisionInfo info;
		if (phys->SegmentCast(l, info)&& info.mActor!=mOwner)
		{	
			//std::cout << typeid(info.mActor).name()<< std::endl;
			//是否碰撞目标
			ShootActor* target = dynamic_cast<ShootActor*>(info.mActor);
			if (target)
			{
				mTargetEnemy = target;
				break;
			}
			else
			{
				FPSActor* player = dynamic_cast<FPSActor*>(info.mActor);
				if (player)
				{
					mTargetEnemy = player;
					break;
				}
			}
			
		}
		i = Math::ToDegrees(i);
	}
}
