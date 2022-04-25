#pragma once
#include "core/MathLibrary.h"

namespace Display
{
class Camera
{
	float fov, aspect, n, f;
	V4 pos, up;
	V4 dir;
	float rad = 0.f;
public:
	Camera(float fov, float ratio, float n, float f);
	V3 getPos();
	void setPos(V4 pos);
	void setRot(V4 dir, float theta);
	M4 pv();
};
}
