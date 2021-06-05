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
	// ���ض���/Ƭ����ɫ��
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// ������ɫ������
	void SetActive();
	// ����Matrix uniform
	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	void SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count);
	// Sets a Vector3 uniform
	void SetVectorUniform(const char* name, const Vector3& vector);
	// Sets a float uniform
	void SetFloatUniform(const char* name, float value);
private:
	// ������ɫ��
	bool CompileShader(const std::string& fileName,
					   GLenum shaderType,
					   GLuint& outShader);
	
	//������ɫ���Ƿ�ɹ�����
	bool IsCompiled(GLuint shader);
	// ���Զ���/Ƭ����ɫ�����������
	bool IsValidProgram();
private:
	// ������ɫ��objectID
	GLuint mVertexShader; //������ɫ��
	GLuint mFragShader;//Ƭ����ɫ��
	GLuint mShaderProgram;//��ɫ������
};
