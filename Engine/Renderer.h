#pragma once
#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <string>
#include <vector>
#include <unordered_map>
#include "Math.h"

struct DirectionalLight
{
	// �ƹⷽ��
	Vector3 mDirection;
	// ������ɫ
	Vector3 mDiffuseColor;
	// ������ɫ 
	Vector3 mSpecColor;
};

class Renderer
{
public:
	Renderer(class Game* game);
	~Renderer();

	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();
	void UnloadData();

	void Draw();


	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void AddMeshComp(class MeshComponent* mesh);
	void RemoveMeshComp(class MeshComponent* mesh);

	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	void SetViewMatrix(const Matrix4& view) { mView = view; }

	void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
	DirectionalLight& GetDirectionalLight() { return mDirLight; }

	Vector3 Unproject(const Vector3& screenPoint) const;
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

	GLFWwindow* GetWindow() const { return window; }

private:
	bool LoadShaders();
	void CreateSpriteVerts();
	void SetLightUniforms(class Shader* shader);

	// Renderer������Game����
	class Game* mGame;
	//����
	GLFWwindow* window;
	// ����ά��
	int mScreenWidth;
	int mScreenHeight;
	// ������ɫ��
	class Shader* mSpriteShader;
	// �����Ѿ����Ƶľ���
	std::vector<class SpriteComponent*> mSprites;
	// �����Ѿ����Ƶ�������
	std::vector<class MeshComponent*> mMeshComps;
	std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;
	// ���ص�����textures Map
	std::unordered_map<std::string, class Texture*> mTextures;
	// ���ص�����Meshes Map
	std::unordered_map<std::string, class Mesh*> mMeshes;
	// ���鶥������
	class VertexArray* mSpriteVerts;
	// ������ɫ��
	class Shader* mMeshShader;
		// Skinned shader
	class Shader* mSkinnedShader;
	//3D��ɫ������ͼͶӰ
	Matrix4 mView;//��ʾ�����λ�úͷ���
	Matrix4 mProjection;
	// ��������
	Vector3 mAmbientLight;
	DirectionalLight mDirLight;



};