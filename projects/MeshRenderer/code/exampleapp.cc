//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <iterator>
// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "render/tiny_gltf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"


using namespace Display;
using namespace tinygltf;
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

			cube = MeshResource::LoadObj("textures/fireHydrant.obj");

			//TextureResource
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/cubepic.png");

			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Translate(V4Zero));

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

	bool BeginParsing(Model& model, const char* gltf_file)
	{
		TinyGLTF loader;
		std::string err;
		std::string warn;

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, gltf_file);
		
		// check if (.glb or .gltf)

		if (!warn.empty())
		{
			printf("Warning: %s\n", warn.c_str());
		}

		if (!err.empty())
		{
			printf("Error: %s\n", err.c_str());
		}

		if (!ret)
		{
			printf("Error: Failed to parse glTF\n");
		}
		return ret;
		// Summary
	}

	struct Info
	{
		uint position;
		uint tangent;
		uint normal;
		uint texels;
		uint indices;
	};

	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		node->getTexture()->LoadFromFile();
		Display::Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 0, -3));
		cam.setRot(V4(0, 1, 0), M_PI);
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		Model model;
		std::vector<Info> info;
		Image texture_img;
		if (!BeginParsing(model, "textures/content/FlightHelmet.gltf"))
		{
			printf("parsing gave up!");
			return;
		}
		// coordinates
		// texture coordinates
		// normals
		// indices
		// materials
		

		size_t meshlen = model.meshes.size();
		for (size_t i = 0; i < meshlen; i++)
		{
			size_t primlen = model.meshes[i].primitives.size();
			for (size_t j = 0; j < primlen; j++)
			{

				// Accessor
				Accessor position = model.accessors[model.meshes[i].primitives[j].attributes["POSITION"]];
				Accessor tangent = model.accessors[model.meshes[i].primitives[j].attributes["TANGENT"]];
				Accessor normal = model.accessors[model.meshes[i].primitives[j].attributes["NORMAL"]];
				Accessor texels = model.accessors[model.meshes[i].primitives[j].attributes["TEXCOORD_0"]];
				Accessor indices = model.accessors[model.meshes[i].primitives[j].indices];
				int materialBufferIndex = model.meshes[i].primitives[j].material;

				// position
				size_t position_byteLength = model.bufferViews[position.bufferView].byteLength;
				size_t position_byteOffset = model.bufferViews[position.bufferView].byteOffset;
				size_t position_byteStride = model.bufferViews[position.bufferView].byteStride;
				size_t position_buffer = model.bufferViews[position.bufferView].buffer;
				

				// tangent
				size_t tangent_byteLength = model.bufferViews[tangent.bufferView].byteLength;
				size_t tangent_byteOffset = model.bufferViews[tangent.bufferView].byteOffset;
				size_t tangent_byteStride = model.bufferViews[tangent.bufferView].byteStride;
				size_t tangent_buffer = model.bufferViews[tangent.bufferView].buffer;


				// normals
				size_t normal_byteLength = model.bufferViews[normal.bufferView].byteLength;
				size_t normal_byteOffset = model.bufferViews[normal.bufferView].byteOffset;
				size_t normal_byteStride = model.bufferViews[normal.bufferView].byteStride;
				size_t normal_buffer = model.bufferViews[normal.bufferView].buffer;


				// texels
				size_t texels_byteLength = model.bufferViews[texels.bufferView].byteLength;
				size_t texels_byteOffset = model.bufferViews[texels.bufferView].byteOffset;
				size_t texels_byteStride = model.bufferViews[texels.bufferView].byteStride;
				size_t texels_buffer = model.bufferViews[texels.bufferView].buffer;
				

				// indices
				size_t indices_byteLength = model.bufferViews[indices.bufferView].byteLength;
				size_t indices_byteOffset = model.bufferViews[indices.bufferView].byteOffset;
				size_t indices_byteStride = model.bufferViews[indices.bufferView].byteStride;
				size_t indices_buffer = model.bufferViews[indices.bufferView].buffer;
				size_t indices_indicesCount = indices.count;
			
				//textures
				tinygltf::TextureInfo baseColorTexture = model.materials[materialBufferIndex].pbrMetallicRoughness.baseColorTexture;
				int w, h, c = model.images[baseColorTexture.index].component;
				if (baseColorTexture.index != -1)
				{
					if (strcmp(model.images[baseColorTexture.index].uri.c_str(), ""))
					{
						unsigned char* temp = stbi_load_from_memory(&model.images[baseColorTexture.index].image[0], model.images[baseColorTexture.index].image.size(), &w, &h, &c, 0);
						for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
						{
							texture_img.image.push_back(temp[i]);
						}
						delete[] temp;
					}
					else
					{
						unsigned char* temp = stbi_load((std::string("texture/content/") + model.images[baseColorTexture.index].uri).c_str(), &w, &h, &c, 0);
						for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
						{
							texture_img.image.push_back(temp[i]);
						}
						delete[] temp;
					}	

					if (stbi_failure_reason())
					{
						std::cerr << "cannot load " << std::string("texture/content/") + model.images[baseColorTexture.index].uri << ": " << stbi_failure_reason() << std::endl;
					}
				}
				else
				{
					unsigned char* temp = stbi_load("texture/default.png", &w, &h, &c, 0);
					for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
					{
						texture_img.image.push_back(temp[i]);
					}
					delete[] temp;
				}
			}
		}
		
		float speed = .08f;

		M4 scene;
		V4 color(1, 1, 1, 1);
		
		while (this->window->IsOpen())
		{
			Em = Em * Translate(Normalize(V4(float(d - a), float(e - q), float(w - s))) * speed);
			scene = cam.pv() * (Em * Evp) * Translate(V4Zero) * Scalar(V4(.1, .1, .1)); // scaling because i can
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			shaderResource->setM4(cam.pv(), "m4ProjViewPos");
			light.bindLight(shaderResource, cam.getPos());
			node->DrawScene(scene, color);
			this->window->SwapBuffers();
		}
	}

} // namespace Example
