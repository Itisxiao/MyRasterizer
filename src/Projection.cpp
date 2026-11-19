
#include "Projection.h"
#include <cmath>
#include <stdexcept>

Projection::Projection(float width, float height, float fovDegrees,
                       float nearPlane, float farPlane)
    : width(width), height(height), nearPlane(nearPlane), farPlane(farPlane)
{
    if (width <= 0.0f || height <= 0.0f || fovDegrees <= 0.0f || fovDegrees >= 180.0f ||
        nearPlane <= 0.0f || farPlane <= nearPlane) {
        throw std::invalid_argument("Invalid projection parameters");
    }
}

ScreenVertex Projection::project(const Vertex& vertex, const Matrix4& projectionMatrix) const
{
    if (vertex.z < nearPlane || vertex.z > farPlane) {
        throw std::runtime_error("Vertex is outside the projection depth range");
    }

    const Vector4 clip = projectionMatrix * Vector4{
        vertex.x, vertex.y, vertex.z, 1.0f
    };
    if (clip.w <= 0.0f) throw std::runtime_error("Vertex is behind camera");

    ScreenVertex result;
    // 透视除法后转成以屏幕中心为原点的像素单位。
    result.x = (clip.x / clip.w) * (width * 0.5f);
    result.y = (clip.y / clip.w) * (height * 0.5f);
    // 光栅器用相机空间 z 做透视校正插值和深度比较。
    result.z = vertex.z;
    result.u = vertex.u;
    result.v = vertex.v;

    return result;
}
