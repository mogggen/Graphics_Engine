#include "config.h"
#include "render/GraphicNode.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <render/tiny_gltf.h>


GraphicNode::GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, M4 transform=Identity()) : Geometry(geometry), Texture(texture), Shader(shader), Transform(transform)
{
	Transform = Translate(V4());
}


std::shared_ptr<tinygltf::Model> GraphicNode::load_gltf(const std::string& filePath)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	bool result = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);

	if (!warn.empty()) {
		// Handle warnings
	}

	if (!err.empty()) {
		// Handle errors
	}

	return result ? std::make_shared<tinygltf::Model>(model) : nullptr;
}

void GraphicNode::DrawScene(const M4& model, const M4& view, const M4& projection)
{
	Texture->BindTexture();
	Texture->BindNormalMap();

	glUseProgram(this->Shader->program);
	// TODO: update these to match with the shaders fields
	
	//Set mvp
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "model"), 1, GL_TRUE, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "view"), 1, GL_TRUE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "projection"), 1, GL_TRUE, (float*)&projection);


	Geometry->render();
}

std::shared_ptr<MeshResource> GraphicNode::getGeometry() { return Geometry; }
std::shared_ptr<TextureResource> GraphicNode::getTexture() { return Texture; }
std::shared_ptr<ShaderResource> GraphicNode::getShader() { return Shader; }
M4 GraphicNode::getTransform() { return Transform; }

void GraphicNode::setGeometry(std::shared_ptr<MeshResource> geometry) { Geometry = geometry; }
void GraphicNode::setTexture(std::shared_ptr<TextureResource> texture) { Texture = texture; }
void GraphicNode::setShader(std::shared_ptr<ShaderResource> shader) { Shader = shader; }
void GraphicNode::setTransform(M4 transform) { Transform = transform; }