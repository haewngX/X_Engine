#pragma once
#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <string>
#include <vector>
#include <unordered_map>
#include "Math.h"

struct DirectionalLight
{
	// 灯光方向
	Vector3 mDirection;
	// 漫反射色
	Vector3 mDiffuseColor;
	// 镜面颜色 
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

	// Renderer所属的Game对象
	class Game* mGame;
	//窗口
	GLFWwindow* window;
	// 窗口维度
	int mScreenWidth;
	int mScreenHeight;
	// 精灵着色器
	class Shader* mSpriteShader;
	// 所有已经绘制的精灵
	std::vector<class SpriteComponent*> mSprites;
	// 所有已经绘制的网格体
	std::vector<class MeshComponent*> mMeshComps;
	std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;
	// 加载的纹理textures Map
	std::unordered_map<std::string, class Texture*> mTextures;
	// 加载的网格Meshes Map
	std::unordered_map<std::string, class Mesh*> mMeshes;
	// 精灵顶点数组
	class VertexArray* mSpriteVerts;
	// 网格着色器
	class Shader* mMeshShader;
		// Skinned shader
	class Shader* mSkinnedShader;
	//3D着色器的视图投影
	Matrix4 mView;//表示相机的位置和方向
	Matrix4 mProjection;
	// 光照数据
	Vector3 mAmbientLight;
	DirectionalLight mDirLight;



};