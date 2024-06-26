#pragma once
#include "core/MathLibrary.h"

class Camera
{
	float fov, aspect, n, f;
    
    V4 pos;
    V3 front, up, right;
    V3 worldUp;

    float yaw, pitch;
public:
	Camera(float fov, float ratio, float n, float f);
	V3 getPos();
	void setPos(V4 pos);
    void calculateDirections();
    M4 view(bool rhs);
	M4 pvm();
};