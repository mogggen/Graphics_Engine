#pragma once
#include <GL/glew.h>
#include "stb_image.h"
#include <string>

class TextureResource
{
	GLuint texture;
	std::string file;
public:
	GLuint normalMap;
	TextureResource(const std::string& file);
	~TextureResource();
	void Destroy();
	void BindTexture();
	void BindNormalMap();
	void LoadTextureFromFile();
	void LoadNormalMapFromFile(const std::string& normalMapPath);
	void LoadTextureFromModel(const unsigned char* texture, int w, int h, int comp);
	void LoadNormalMapFromModel(const unsigned char* normalMap, int w, int h, int comp);
};