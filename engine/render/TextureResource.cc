#include "config.h"
#include "render/TextureResource.h"

TextureResource::TextureResource(const std::string & _texturePath, const std::string & _normalMapPath, const std::string& _specularMapPath)
{
    LoadTextureFromFile(&diffuseAlbedo, 0, _texturePath);
    LoadTextureFromFile(&normalMap, 1, _normalMapPath);
    LoadTextureFromFile(&specularMap, 2, _specularMapPath);
}

void TextureResource::LoadTextureFromFile(GLuint* texture, unsigned index, const std::string& texturePath)
{
	int imgWidth, imgHeight, nrChannels;

	const unsigned char* img = stbi_load(texturePath.c_str(), &imgWidth, &imgHeight, &nrChannels, STBI_rgb);
	if (img == nullptr)
	{
		printf("%i %u %s: Image loaded incorrectly\n", *texture, index, texturePath.c_str());
		exit(1);
	}

	glGenTextures(1, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (nrChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	}

	else if (nrChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	BindTexture(*texture, index);
}

void TextureResource::LoadTextureFromBuffer(GLuint* texture, unsigned index, const unsigned char* textureBuf, int w, int h, int comp)
{
	glGenTextures(1, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (comp == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuf);
	}

	else if (comp == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuf);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	BindTexture(*texture, index);
}

void TextureResource::BindTexture(GLuint textureHandle, unsigned index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

    glActiveTexture(GL_TEXTURE0);
}

TextureResource::~TextureResource()
{
	Destroy();
}

void TextureResource::Destroy()
{
	glDeleteTextures(1, &diffuseAlbedo);
	glDeleteTextures(1, &normalMap);
    glDeleteTextures(1, &specularMap);
}