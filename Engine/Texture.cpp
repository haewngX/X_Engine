#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <SDL.h>

Texture::Texture()
:mTextureID(0)
,mWidth(0)
,mHeight(0)
{
	
}

Texture::~Texture()
{
	
}

bool Texture::Load(const std::string& fileName)
{
	int channels = 0;
	
	unsigned char* image = stbi_load(fileName.c_str(),
										   &mWidth, &mHeight, &channels, 0);
	
	if (image == nullptr)
	{
		printf("failed to load image %s", fileName.c_str());
		return false;
	}
	
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}
	
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format,
				 GL_UNSIGNED_BYTE, image);
	
	stbi_image_free(image);
	
	// Enable linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;

	// Generate a GL texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, surface->pixels);

	// Use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
