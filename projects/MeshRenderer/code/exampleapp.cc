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

			model = GraphicNode::load_gltf("textures/Avocado.glb");
			model = GraphicNode::load_gltf("textures/Avocado.gltf");
			//cube = MeshResource::LoadObj("textures/cube.obj");
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/test.jpg");
			
			//TextureResource
			
			if (model != nullptr)
			{
				cube = MeshResource::LoadGLTF(static_cast<const tinygltf::Model&>(*model));
				
				if (!model->materials.empty())
				{
					const tinygltf::Material& material = model->materials[0];
					const int baseColorIndex = material.pbrMetallicRoughness.baseColorTexture.index;
					if (baseColorIndex != -1)
					{
						const unsigned char* texBuf = (const unsigned char*)(&model->images[baseColorIndex].image[0]);
						int texW = model->images[baseColorIndex].width;
						int texH = model->images[baseColorIndex].height;
						int texComp = model->images[baseColorIndex].component;
						texture->LoadTextureFromModel(texBuf, texW, texH, texComp);
					}
					else
					{
						texture->LoadTextureFromFile();
					}

					const int normalMapIndex = material.normalTexture.index;
					if (normalMapIndex != -1)
					{
						const unsigned char* normalBuf = (const unsigned char*)(&model->images[normalMapIndex].image[0]);
						int normalW = model->images[normalMapIndex].width;
						int normalH = model->images[normalMapIndex].height;
						int normalComp = model->images[normalMapIndex].component;
						texture->LoadNormalMapFromModel(normalBuf, normalW, normalH, normalComp);
					}
					else
					{
						texture->LoadNormalMapFromFile("textures/perfect.jpg");
					}
				}
				else
				{
					texture->LoadTextureFromFile();
					texture->LoadNormalMapFromFile("textures/perfect.jpg");
				}
			}


			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Translate(V4Zero));

			if (model != nullptr)
			{
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
		
		V3 timeOfDay;
		Lightning light(timeOfDay, V3(1, 1, 1), .01f);
		
		float speed = .008f;

		while (this->window->IsOpen())
		{
			timeOfDay = V3(10 * cosf(frameIndex / 100.f), -3, 10 * sinf(frameIndex / 100.f));
			light.setPos(timeOfDay);
			Em = Em * Translate(Normalize(V4(float(d - a), float(e - q), float(w - s))) * speed);
			//scene = cam->pv() * (Em * Evp) * Translate(V4Zero);// *Scalar(V4(10, 10, 10)); // scaling because i can
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			light.bindLight(shaderResource, cam->getPos(), node->getTexture()->normalMap);
			node->DrawScene(Em * Translate(V4(0, 0, -1, 1)) * Scalar(V4(10, 10, 10, 1)), Evp, cam->pv());
			this->window->SwapBuffers();
			frameIndex++;
		}
	}

} // namespace Example