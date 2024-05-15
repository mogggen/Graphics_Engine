#include "config.h"
#include "Lighting.h"


Lighting::Lighting(V3 _pos, V3 _rgb, float _intensity)
{
    pos = _pos;
    rgb = _rgb;
    intensity = _intensity;
}

Lighting::~Lighting()
{

}


void Lighting::setPos(V3 _pos)
{
    pos = _pos;
}

V3 Lighting::getPos()
{
    return pos;
}

void Lighting::setColor(V3 _rgb)
{
    rgb = _rgb;
}

V3 Lighting::getColor()
{
    return rgb;
}

void Lighting::setIntensity(float _intensity)
{
    intensity = _intensity;
}

float Lighting::getIntensity()
{
    return intensity;
}

void Lighting::bindLight(const std::shared_ptr<ShaderResource> shader, V3 cameraPos, GLuint normalMapId)
{
    // TODO: send right Parameters
    shader->setInt(normalMapId, "normalMap");
    shader->setV3(rgb, "lightColor");
    shader->setV3(pos, "lightPos");
    shader->setFloat(1, "lightIntensity");

    shader->setV3(cameraPos, "camPos");
    shader->setFloat(1, "specularIntensity");
}
