//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace Display;
namespace Example
{
	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::~ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/

	bool
		ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		//assign ExampleApp variables
		w = a = s = d = q = e = false;
		window->GetSize(width, height);
		Em = Evp = Translate(V4());
		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
		{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;

			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
			}
		});

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
		{
			isRotate = button == GLFW_MOUSE_BUTTON_1 && action;
			if (!isRotate)
			{
				prevX = senseX;
				prevY = senseY;
			}
		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
			if (isRotate)
			{
				senseX = prevX + (0.002 * (x - width / 2));
				senseY = prevY + (0.002 * (y - height / 2));
				Evp = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			}
		});
		

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			//MeshResource

			std::shared_ptr<tinygltf::Model> model = GraphicNode::load_gltf("textures/Avocado.gltf");
			//cube = MeshResource::LoadObj("textures/cube.obj");
			cube = MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*model));

			//TextureResource
			std::shared_ptr<TextureResource> texture;
			
			for (const tinygltf::Material& material : model->materials)
			{
				const int baseColorIndex = material.pbrMetallicRoughness.baseColorTexture.index;
				const int normalMapIndex = material.normalTexture.index;
				texture = std::make_shared<TextureResource>("textures/" + model->images[baseColorIndex].uri);

				texture->LoadTextureFromFile();
				texture->LoadNormalMapFromFile("textures/" + model->images[normalMapIndex].uri);
				//const unsigned char* texBuf = (const unsigned char*)(&model->images[baseColorIndex].image[0]);
				//int texW = model->images[baseColorIndex].width;
				//int texH = model->images[baseColorIndex].height;
				//int texComp = model->images[baseColorIndex].component;
				//texture->LoadTextureFromModel(texBuf, texW, texH, texComp);

				//const unsigned char* normalBuf = (const unsigned char*)(&model->images[baseColorIndex].image[0]);
				//int normalW = model->images[normalMapIndex].width;
				//int normalH = model->images[normalMapIndex].height;
				//int normalComp = model->images[normalMapIndex].component;
				//texture->LoadNormalMapFromModel(normalBuf, normalW, normalH, normalComp);
			}

			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Translate(V4Zero));

			for (const tinygltf::Camera& camera : model->cameras)
			{
				if (camera.type == "perspective")
				{
					const tinygltf::PerspectiveCamera& per = camera.perspective;
					per.yfov;
					per.aspectRatio;
					per.znear;
					per.zfar;
					cam = std::make_shared<Camera>(per.yfov, per.aspectRatio, per.znear, per.zfar);
				}
			}

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
			std::cout << '(';
			for (char i = 0; i < 4; i++)
				std::cout << round(v.data[i]) << (i == 3 ? ")\n" : ", ");
		}
	}

	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		if (cam == nullptr)
		{
			cam = std::make_shared<Camera>(90, (float)width / height, 0.01f, 1000.0f);
		}
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		//std::shared_ptr<MeshResource> mm = std::make_shared<MeshResource>(nullptr);

		float speed = .008f;

		while (this->window->IsOpen())
		{
			Em = Em * Translate(Normalize(V4(float(d - a), float(e - q), float(w - s))) * speed);
			//scene = cam->pv() * (Em * Evp) * Translate(V4Zero);// *Scalar(V4(10, 10, 10)); // scaling because i can
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			light.bindLight(shaderResource, cam->getPos(), node->getTexture()->normalMap);
			node->DrawScene(Em * Translate(V4(0, 0, -1, 1)) * Scalar(V4(10, 10, 10, 1)), Evp, cam->pv());
			this->window->SwapBuffers();
		}
	}

} // namespace Example