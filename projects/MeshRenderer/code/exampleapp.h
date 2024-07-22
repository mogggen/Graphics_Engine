#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/Vertex.h"
#include "render/MeshResource.h"
#include "render/Camera.h"
#include "render/Lightning.h"
#include "render/ShaderResource.h"
#include "render/GraphicNode.h"
#include "render/window.h"

namespace Example
{
	class ExampleApp : public Core::App
	{
	public:
		ExampleApp();
		~ExampleApp();
		bool Open();
		void Run();
	private:
		size_t frameIndex = 0;
		int width, height;
		float64 prevX = 0, prevY = 0;
		float64 senseX = 0, senseY = 0;
		bool w, a, s, d,
			q, e, isRotate = false;

		std::shared_ptr<tinygltf::Model> model;
		std::shared_ptr<Camera> cam;

        std::shared_ptr<MeshResource> mesh;
        std::shared_ptr<TextureResource> texture;
        std::shared_ptr<ShaderResource> shaderGeometryPass;
        std::shared_ptr<ShaderResource> shaderLightingPass;
        std::shared_ptr<ShaderResource> shaderLightBoxPass;

		Display::Window* window;
	};
} // namespace Example