#pragma once
#include <GL/glew.h>
#include "stb_image.h"
#include <string>

class TextureResource
{
	void BindTexture(GLuint textureHandle, unsigned index);
	void Destroy();
public:
	GLuint diffuseAlbedo;
	GLuint normalMap;
    GLuint specularMap;
	TextureResource(const std::string & _texturePath, const std::string & _normalMapPath, const std::string& _specularMapPath);
	~TextureResource();
	void LoadTextureFromFile(GLuint* textureHandle, unsigned index, const std::string& texturePath);
	void LoadTextureFromBuffer(GLuint* textureHandle, unsigned index, const unsigned char* texture, int w, int h, int comp);
};