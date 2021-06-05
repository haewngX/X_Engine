#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <string>
#include "Math.h"

class Shader
{
public:
	Shader();
	~Shader();
	// 加载顶点/片段着色器
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// 激活着色器程序
	void SetActive();
	// 设置Matrix uniform
	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	void SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count);
	// Sets a Vector3 uniform
	void SetVectorUniform(const char* name, const Vector3& vector);
	// Sets a float uniform
	void SetFloatUniform(const char* name, float value);
private:
	// 编译着色器
	bool CompileShader(const std::string& fileName,
					   GLenum shaderType,
					   GLuint& outShader);
	
	//测试着色器是否成功编译
	bool IsCompiled(GLuint shader);
	// 测试定点/片段着色器的链接情况
	bool IsValidProgram();
private:
	// 储存着色器objectID
	GLuint mVertexShader; //顶点着色器
	GLuint mFragShader;//片段着色器
	GLuint mShaderProgram;//着色器程序
};
