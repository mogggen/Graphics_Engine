#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <render/tiny_gltf.h>

class MeshResource
{
	GLint indices;
	GLuint vertexBuffer;
	GLuint indexBuffer;
public:
	static std::shared_ptr<MeshResource> Cube();
	static std::shared_ptr<MeshResource> LoadObj(const char* pathToFile);
	static std::shared_ptr<MeshResource> LoadGLTF(const tinygltf::Model& model);
	MeshResource(Vertex vertices[], uint32_t verticeslength, uint32_t indices[], uint32_t indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};