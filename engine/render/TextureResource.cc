#include "config.h"
#include "render/TextureResource.h"

TextureResource::TextureResource(const std::string& file) : file(file)
{

}

void TextureResource::LoadTextureFromFile()
{
	int imgWidth, imgHeight, nrChannels;

	const unsigned char* img = stbi_load(file.c_str(), &imgWidth, &imgHeight, &nrChannels, STBI_rgb);
	if (img == nullptr)
	{
		printf("Image loaded incorrectly");
		exit(1);
	}

	glGenTextures(1, &texture);

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

	BindTexture();
}

void TextureResource::LoadNormalMapFromFile(const std::string& normalMapPath)
{
	int imgWidth, imgHeight, nrChannels;

	const unsigned char* img = stbi_load(normalMapPath.c_str(), &imgWidth, &imgHeight, &nrChannels, STBI_rgb);
	if (img == nullptr)
	{
		printf("Image loaded incorrectly");
		exit(1);
	}

	glGenTextures(1, &normalMap);

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

	BindNormalMap();
}

void TextureResource::LoadTextureFromModel(const unsigned char* textureBuf, int w, int h, int comp)
{
	glGenTextures(1, (GLuint*)(&texture));

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

	BindTexture();
}
void TextureResource::LoadNormalMapFromModel(const unsigned char* normalMapBuf, int w, int h, int comp)
{
	glGenTextures(1, (GLuint*)(&normalMap));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (comp == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, normalMapBuf);
	}

	else if (comp == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalMapBuf);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	BindTexture();
}


void TextureResource::BindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void TextureResource::BindNormalMap()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
}

TextureResource::~TextureResource()
{
	Destroy();
}

void TextureResource::Destroy()
{
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &normalMap);
}