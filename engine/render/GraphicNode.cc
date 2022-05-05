#include "config.h"
#include "render/GraphicNode.h"


GraphicNode::GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, M4 transform=Identity()) : Geometry(geometry), Texture(texture), Shader(shader), Transform(transform)
{
	Transform = Translate(V4());
}

void GraphicNode::DrawScene(M4& mvp, V4& rgba)
{
	Texture->BindTexture();

	glUseProgram(this->Shader->program);

	//Set matrix
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "m4Pos"), 1, GL_TRUE, (float*)&mvp);

	//set colorVector
	glUniform4fv(glGetUniformLocation(Shader->program, "colorVector"), 1, (float*)&rgba);

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