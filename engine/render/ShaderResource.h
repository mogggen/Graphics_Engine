#pragma once
#include <GL/glew.h>
#include <fstream>
#include "core/MathLibrary.h"

struct ShaderResource
{
    std::string vsPath;
    std::string psPath;
	GLchar* vs;
	GLchar* ps;
	GLuint program;

	public:
	ShaderResource(std::string vsPath, std::string psPath);
	~ShaderResource();
	void LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath);
	void bindShaderResource();
	
	void setFloat(float facIn, std::string parameterName);

	void setInt(int idIn, std::string parameterName);

	void setV1(float floatIn, std::string parameterName);
	void setV2(V2 vecIn, std::string parameterName);
	void setV3(V3 vecIn, std::string parameterName);
	void setV4(V4 vecIn, std::string parameterName);

	void setM4(M4 matIn, std::string parameterName);

};