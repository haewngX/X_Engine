#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Actor.h"
#include "Game.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner, bool isSkeletal)
	:Component(owner)
	,mMesh(nullptr)
	,mTextureIndex(0)
	,mVisible(true)
	,mIsSkeletal(isSkeletal)
{
	mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform
		shader->SetMatrixUniform("uWorldTransform", 
			mOwner->GetWorldTransform());
		// Set specular power
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// Set the active texture
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}
		// Set the mesh's vertex array as active
		if (mMesh->mVertexArray)
		{
			VertexArray* va = mMesh->mVertexArray;
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}

		for (int i = 0; i < mMesh->m_Entries.size(); i++)
		{
			VertexArray* va = mMesh->m_Entries[i].mVertexArray;
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
		
	}
}
