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

	// �״�ĽǶȷ�Χ�Ͱ뾶Ƶ��
	float mRadarRange;
	float RangeStep = 10.0f;
	float mRadarRadius;
	// �Ƿ񿴵���Ŀ��
	class Actor* mTargetEnemy;

};