#pragma once
#include "Math.h"
#include "Component.h"
#include <vector>


class Radar : public Component
{
public:
	Radar(class Actor* owner, int updateOrder = 200, float RadarRange = 60, float RadarRadius = 5000);

	void Update(float deltaTime) override;
	class Actor* GetTargetEnemy() {return mTargetEnemy;}

private:

	// 雷达的角度范围和半径频率
	float mRadarRange;
	float RangeStep = 10.0f;
	float mRadarRadius;
	// 是否看到了目标
	class Actor* mTargetEnemy;

};