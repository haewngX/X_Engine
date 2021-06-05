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
	// 将视图矩阵参数传给渲染器
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
}
