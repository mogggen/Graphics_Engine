#include "config.h"
#include "render/Camera.h"

Camera::Camera(float fov, float aspect, float n, float f) : fov(fov), aspect(aspect), n(n), f(f)
{
    pos = V4(0, 0, 0);
    yaw = -90.f;
    calculateDirections();
}

V3 Camera::getPos()
{
    return pos.toV3();
}

void Camera::setPos(V4 pos)
{
    this->pos = pos;
    calculateDirections();
}


M4 lookAtRH(V3 eye, V3 center, V3 up)
{
    V3 f(Normalize(center - eye));
    V3 s(Normalize(Cross(f, up)));
    V3 u(Cross(s, f));

    M4 Result = Identity();
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;
    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;
    Result[0][2] = -f.x;
    Result[1][2] = -f.y;
    Result[2][2] = -f.z;
    Result[3][0] = -Dot(s, eye);
    Result[3][1] = -Dot(u, eye);
    Result[3][2] = Dot(f, eye);
    return Result;
}


M4 lookAtLH(V3 eye, V3 center, V3 up)
{
    V3 f(Normalize(center - eye));
    V3 s(Normalize(Cross(up, f)));
    V3 u(Cross(f, s));

    M4 Result = Identity();
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;
    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;
    Result[0][2] = f.x;
    Result[1][2] = f.y;
    Result[2][2] = f.z;
    Result[3][0] = -Dot(s, eye);
    Result[3][1] = -Dot(u, eye);
    Result[3][2] = -Dot(f, eye);
    return Result;
}

void Camera::calculateDirections()
{
    V3 _front;
    _front.x = cosf(yaw) * cosf(pitch);
    _front.y = sinf(pitch);
    _front.z = sinf(yaw) * cosf(pitch);
    front = Normalize(front);
    right = Normalize(Cross(front, worldUp));
    up = Normalize(Cross(right, front));
}

M4 Camera::view(bool rhs)
{
    if (rhs)
    {
        return lookAtRH(pos.toV3(), pos.toV3() + front, up);
    }
    else
    {
        return lookAtLH(pos.toV3(), pos.toV3() + front, up);
    }
}

M4 Camera::pvm(bool rhsLookAt)
{
    return projection(fov, aspect, n, f) *
        view(true) *
        Rotation(V3(0, 1, 0), yaw) *
        Rotation(V3(0, 0, 1), pitch) * 
        Translate(pos);
}
