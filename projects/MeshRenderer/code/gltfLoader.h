#include <string>
#include <vector>
#include "core/MathLibrary.h"

#define TINYGLTF_IMPLEMENTATION
#include "render/tiny_gltf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"

struct Info{
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint texture;
    GLuint normalMap;
    V4 color;
    int componentType;
    int posByteStride;
    int posByteLength;
    int texByteStride;
    int texByteLength;
    int normalByteStride;
    int normalByteLength;
    int indices;
    int tangentByteStride;
    int tangentByteLength;
};

using namespace tinygltf;

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


void load_gltf(std::vector<Info>& info)
{
    Model model;
    Image texture_img;
    
    if (!BeginParsing(model, "textures/content/FlightHelmet.gltf"))
    {
        printf("parsing gave up!");
        return;
    }
 
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
                if (!strcmp(model.images[baseColorTexture.index].uri.c_str(), ""))
                {
                    unsigned char* temp = stbi_load_from_memory(&model.images[baseColorTexture.index].image[0], model.images[baseColorTexture.index].image.size(), &w, &h, &c, 0);
                    for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
                    {
                        texture_img.image.push_back(temp[i]);
                    }
                    delete[] temp;

                    if (stbi_failure_reason())
                    {
                        std::cerr << "cannot load Image from memory: " << stbi_failure_reason() << std::endl;
                    }
                }
                else
                {
                    unsigned char* temp = stbi_load((std::string("textures/content/") + model.images[baseColorTexture.index].uri).c_str(), &w, &h, &c, 0);
                    for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
                    {
                        texture_img.image.push_back(temp[i]);
                    }
                    delete[] temp;

                    if (stbi_failure_reason())
                    {
                        std::cerr << "cannot load " << std::string("textures/content/") + model.images[baseColorTexture.index].uri << ": " << stbi_failure_reason() << std::endl;
                    }
                }	

                // sampler skit
            }
            else
            {
                unsigned char* temp = stbi_load("textures/default.png", &w, &h, &c, 0);
                for (size_t i = 0; i < w*h*c/*sizeof(temp) / sizeof(unsigned char*)*/; i++)
                {
                    texture_img.image.push_back(temp[i]);
                }
                delete[] temp;
            }
        }

        // gl grejer

        info.push_back(
        {
            
        });
    }
}

void render_gltf(const std::vector<Info>& info)
{
    for (size_t i = 0; i < info.size(); i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, info[i].vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info[i].indexBuffer);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 7));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 9));

        glDrawElements(GL_TRIANGLES, info[i].indices, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
