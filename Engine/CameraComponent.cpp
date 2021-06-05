#include "CameraComponent.h"
#include "Actor.h"
#include "Renderer.h"
#include "Game.h"

CameraComponent::CameraComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
{
	

}

void CameraComponent::SetViewMatrix(const Matrix4& view)
{
	// ����ͼ�������������Ⱦ��
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
}
