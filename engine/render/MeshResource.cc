#include "config.h"
#include "render/MeshResource.h"
#include <stdio.h>
#include <inttypes.h>

MeshResource::MeshResource(Vertex vertices[], uint32_t Verticeslength, uint32_t indices[], uint32_t indicesLength) : indices(indicesLength)
{
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Verticeslength, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &this->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesLength, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 7));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 9));

	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<MeshResource> MeshResource::LoadObj(const char *pathToFile)
{
	char buf[1024];
	FILE *fs;
#ifndef __linux
	fopen_s(&fs, pathToFile, "r"); // textures/sphere.obj
#else
	fs = fopen64(pathToFile, "r"); // "textures/sphere.obj"
#endif

	unsigned long long verticesUsed = 0ull;
	std::vector<uint32_t> indices;
	std::vector<V3> coords;
	std::vector<V2> texels;
	std::vector<V3> normals;
	std::vector<Vertex> vertices; // complete package

	if (fs)
	{
		while (true)
		{
			int foo = fscanf(fs, "%1024s", buf); // reads one word at a time and places it at buf[0] with a trailing '\0'
			if (foo <= 0)
			{
				break;
			}

			if (buf[0] == 'v' && buf[1] == '\0')
			{
				V3 nextCoordinate;
				if (fscanf(fs, "%f %f %f", &nextCoordinate.x, &nextCoordinate.y, &nextCoordinate.z) == 3)
				{
					coords.push_back(nextCoordinate);
				}
				else
				{
					std::cerr << "missing arguments in vertex, expected 3" << std::endl;
				}
			}

			else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == '\0')
			{
				V2 nextTexel;
				if (fscanf(fs, "%f %f", &nextTexel.x, &nextTexel.y) == 2)
				{
					texels.push_back(nextTexel);
				}
				else
				{
					std::cerr << "missing arguments in texel, expected 2" << std::endl;
				}
			}

			else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == '\0')
			{
				V3 nextNormal;
				if (fscanf(fs, "%f %f %f", &nextNormal.x, &nextNormal.y, &nextNormal.z) == 3)
				{
					normals.push_back(nextNormal);
				}
				else
				{
					std::cerr << "missing arguments in texel, expected 3" << std::endl;
				}
			}

			else if (buf[0] == 'f' && buf[1] == '\0')
			{
				char pos[4][64];
				uint8_t argc = fscanf(fs, "%s %s %s %s", &pos[0], &pos[1], &pos[2], &pos[3]);

				uint32_t listOfIndices[4][3];

				if (argc == 4 && pos[3][0] != 'f' && pos[3][0] != '#')
				{
					for (size_t i = 0; i < 4; i++)
					{
						if (sscanf(pos[i], "%lu"
										   "/ %lu"
										   "/ %lu"
										   "/",
								   &listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]) != 3)
							break;
							vertices.push_back(Vertex{
								coords[(listOfIndices[i][0]) - 1],
								normals[(listOfIndices[i][2]) - 1],
								texels[(listOfIndices[i][1]) - 1],
								V3(1, 1, 1),
								V3(1, 1, 1)
							});
					}

					float dist1 = (vertices[vertices.size() - 4].pos - vertices[vertices.size() - 2].pos).Length();
					float dist2 = (vertices[vertices.size() - 3].pos - vertices[vertices.size() - 1].pos).Length();
					if (dist1 > dist2)
					{
						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 1);

						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 2);
						indices.push_back(vertices.size() - 1);
					}
					else
					{
						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 2);

						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 2);
						indices.push_back(vertices.size() - 1);
					}
				}
				else if (argc == 3)
				{
					for (size_t i = 0; i < 3; i++)
					{
						if (sscanf(pos[i], "%lu"
										   "/ %lu"
										   "/ %lu"
										   "/",
								   &listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]) != 3)
								   break;

							vertices.push_back(Vertex{
								coords[listOfIndices[i][0] - 1],
								normals[listOfIndices[i][2] - 1],
								texels[listOfIndices[i][1] - 1],
								V3(1, 1, 1),
								V3(1, 1, 1),
							});
						indices.push_back(vertices.size() - 1);
					}
				}
			}
		}
	}
	else
	{
		printf("file not found with path \"./%s\"\n", pathToFile);
	}
	if (fs)
		fclose(fs);
	std::cout << "loaded " << pathToFile << std::endl;
	
	return std::make_shared<MeshResource>(&vertices[0], vertices.size(), &indices[0], indices.size());
}

std::shared_ptr<MeshResource> MeshResource::LoadGLTF(const tinygltf::Model& model)
{
	std::vector<Vertex> resVert; // complete

	std::vector<float> tempPositionBuffer;
	std::vector<float> tempNormalBuffer;
	std::vector<float> tempTangentBuffer;
	std::vector<float> tempTexelBuffer;

	std::vector<V3> position3;
	std::vector<V3> tangent3;
	std::vector<V3> normal3;
	std::vector<V2> texel2;

	std::vector<unsigned int> tri_indices; // Store triangle indices

	for (const tinygltf::Buffer& buffer : model.buffers)
	{
		std::vector<char> binaryData;
		//tinygltf::DecodeDataURI();
		if (!buffer.uri.empty())
		{
			std::ifstream binFile("textures/" + buffer.uri, std::ios::binary | std::ios::ate);

			if (binFile.is_open())
			{
				// Get the file size and allocate a buffer
				std::streamsize fileSize = binFile.tellg();
				binaryData.resize(fileSize);

				// Seek back to the beginning and read the binary data
				binFile.seekg(0, std::ios::beg);
				binFile.read(binaryData.data(), fileSize);
				binFile.close();

				// Now, you can use binaryData as your binary buffer
			}
		}
		else
		{
			assert(!buffer.data.empty());
			binaryData = (std::vector<char>&)(buffer.data);
		}

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
				{
					std::cerr << "this parser only handles triangle mode" << std::endl;
					return nullptr;
				}

				// Access the accessor for position data
				int positionAccessorIndex = primitive.attributes.at("POSITION");
				const tinygltf::Accessor& positionAccessor = model.accessors[positionAccessorIndex];
				const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
				const char* positionData = &binaryData[positionBufferView.byteOffset];

				// Access the accessor for normals
				int normalAccessorIndex = primitive.attributes.at("NORMAL");
				const tinygltf::Accessor& normalAccessor = model.accessors[normalAccessorIndex];
				const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
				const char* normalData = &binaryData[normalBufferView.byteOffset];


				//int tangentAccessorIndex = primitive.attributes.at("TANGENT");
				//const tinygltf::Accessor& tangentAccessor = model.accessors[tangentAccessorIndex];
				//const tinygltf::BufferView& tangentBufferView = model.bufferViews[tangentAccessor.bufferView];
				//const char* tangentData = &binaryData[tangentBufferView.byteOffset];

				// Access the accessor for normals
				int texelAccessorIndex = primitive.attributes.at("TEXCOORD_0");
				const tinygltf::Accessor& texelAccessor = model.accessors[texelAccessorIndex];
				const tinygltf::BufferView& texelBufferView = model.bufferViews[texelAccessor.bufferView];
				const char* texelData = &binaryData[texelBufferView.byteOffset];


				// Access the accessor for indices (triangles)
				int indicesAccessorIndex = primitive.indices;
				const tinygltf::Accessor& indicesAccessor = model.accessors[indicesAccessorIndex];
				const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
				const  char* indicesData = &binaryData[indicesBufferView.byteOffset];

				// Create lists to store vertices and indices

				// Iterate through position data and convert to vertices
				for (size_t i = 0; i < positionAccessor.count; ++i) {
					const void* positionPtr = positionData + i * positionAccessor.ByteStride(positionBufferView);

					// Assuming positions are 3D (x, y, z)
					const float* position = reinterpret_cast<const float*>(positionPtr);
					for (int j = 0; j < 3; ++j) {
						tempPositionBuffer.push_back(position[j]);
					}
				}

				for (size_t i = 0; i < normalAccessor.count; ++i) {
					const void* normalPtr = normalData + i * normalAccessor.ByteStride(normalBufferView);

					// Assuming normals are 3D (x, y, z)
					const float* normal = reinterpret_cast<const float*>(normalPtr);
					for (int j = 0; j < 3; ++j) {
						tempNormalBuffer.push_back(normal[j]);
					}
				}

				//for (size_t i = 0; i < tangentAccessor.count; ++i) {
				//	const void* tangentPtr = tangentData + i * tangentAccessor.ByteStride(tangentBufferView);

				//	// Assuming tangents are 3D (x, y, z)
				//	const float* tangent = reinterpret_cast<const float*>(tangentPtr);
				//	for (int j = 0; j < 3; ++j) {
				//		tempTangentBuffer.push_back(tangent[j]);
				//	}
				//}

				for (size_t i = 0; i < texelAccessor.count; ++i) {
					const void* texelPtr = texelData + i * texelAccessor.ByteStride(texelBufferView);

					// Assuming texel are 2D (u, v)
					const float* texel = reinterpret_cast<const float*>(texelPtr);
					for (int j = 0; j < 2; ++j) {
						tempTexelBuffer.push_back(texel[j]);
					}
				}

				for (size_t i = 0; i < tempPositionBuffer.size(); i += 3)
				{
					position3.push_back(V3(
						tempPositionBuffer[i + 0],
						tempPositionBuffer[i + 1],
						tempPositionBuffer[i + 2]
					));
				}

				for (size_t i = 0; i < tempNormalBuffer.size(); i += 3)
				{
					normal3.push_back(V3(
						tempNormalBuffer[i + 0],
						tempNormalBuffer[i + 1],
						tempNormalBuffer[i + 2]
					));
				}

				for (size_t i = 0; i < tempTangentBuffer.size(); i += 3)
				{
					tangent3.push_back(V3(
						tempTangentBuffer[i + 0],
						tempTangentBuffer[i + 1],
						tempTangentBuffer[i + 2]
					));
				}

				for (size_t i = 0; i < tempTexelBuffer.size(); i += 2)
				{
					texel2.push_back(V2(
						tempTexelBuffer[i + 0],
						tempTexelBuffer[i + 1]
					));
				}

				assert(texel2.size() == normal3.size() && normal3.size() == position3.size() && normal3.size());

				for (size_t i = 0; i < position3.size(); ++i)
				{
					resVert.push_back(
						{
							position3[i],
							normal3.empty() ? V3(0.5, 0.5, 0.5) : normal3[i],
							texel2.empty() ? V2(1, 1) : texel2[i],
							tangent3.empty() ? V3(1, 1, 1) : tangent3[i],
							V3(),
						});
				}

				// Iterate through indices data and convert to indices
				for (size_t i = 0; i < indicesAccessor.count; i += 3) {
					const void* indexPtr = indicesData + i * indicesAccessor.ByteStride(indicesBufferView);
					if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						const unsigned short* index = reinterpret_cast<const unsigned short*>(indexPtr);
						tri_indices.push_back(index[0]);
						tri_indices.push_back(index[1]);
						tri_indices.push_back(index[2]);
					}
					else if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const unsigned int* index = reinterpret_cast<const unsigned int*>(indexPtr);
						tri_indices.push_back(index[0]);
						tri_indices.push_back(index[1]);
						tri_indices.push_back(index[2]);
					}
				}

				//for (size_t ss = 0; ss < tri_indices.size(); ss++)
				//{
				//	auto first = std::find(tri_indices.begin(), tri_indices.end(), tri_indices[ss]);
				//	auto pos = std::find(first + 1, tri_indices.end(), tri_indices[ss]);
				//	if (pos == tri_indices.end()) continue;

				//	tri_indices.erase(pos);
				//	ss = 0;
				//}
			

			}
		}
	}
	std::cout << "loaded " << model.meshes[0].name << std::endl;
	return std::make_shared<MeshResource>(&resVert[0], resVert.size(), &tri_indices[0], tri_indices.size());
}

/// <summary>
/// Destroy glBuffers
/// </summary>
void MeshResource::Destroy()
{
	glDeleteBuffers(1, &this->indexBuffer);
	glDeleteBuffers(1, &this->vertexBuffer);
}

MeshResource::~MeshResource()
{
	Destroy();
}