#pragma once
#include <memory>
#include "core/MathLibrary.h"
#include "render/MeshResource.h"
#include "render/TextureResource.h"
#include "render/ShaderResource.h"

class GraphicNode
{
	std::shared_ptr<MeshResource> Geometry;
	std::shared_ptr<TextureResource> Texture;
	std::shared_ptr<ShaderResource> Shader;
	M4 Transform;
public:
	GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, M4 transform);
	void DrawScene(M4& mvp, V4& rbga);

	std::shared_ptr<MeshResource> getGeometry();
	std::shared_ptr<TextureResource> getTexture();
	std::shared_ptr<ShaderResource> getShader();
	M4 getTransform();

	void setGeometry(std::shared_ptr<MeshResource> geometry);
	void setTexture(std::shared_ptr<TextureResource> texture);
	void setShader(std::shared_ptr<ShaderResource> shader);
	void setTransform(M4 transform);
};