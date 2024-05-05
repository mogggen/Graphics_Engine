#pragma once
#include "core/MathLibrary.h"
#include "ShaderResource.h"
#include "GraphicNode.h"
#include <memory>

class Lighting
{
    std::shared_ptr<GraphicNode> sphere;
    V3 pos;
    V3 rgb;
    float intensity;
public:
    Lighting() = default;
    Lighting(V3 _pos, V3 _rgb, float _intensity);
    ~Lighting();

    void SetModel();
    std::shared_ptr<GraphicNode> GetModel();

    void setPos(V3 _pos);
    V3 getPos();

    void setColor(V3 _rgb);
    V3 getColor();

    void setIntensity(float _intensity);
    float getIntensity();

    void bindLight(std::shared_ptr<ShaderResource> shader, V3 cameraPos, GLuint normalMapId);
};
