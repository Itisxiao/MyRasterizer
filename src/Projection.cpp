
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

ClipVertex Projection::toClipSpace(const Vertex& vertex, const Matrix4& projectionMatrix) const
{
    const Vector4 clip = projectionMatrix * Vector4{
        vertex.x, vertex.y, vertex.z, 1.0f
    };
    return {clip.x, clip.y, clip.z, clip.w, vertex.z, vertex.u, vertex.v};
}

ScreenVertex Projection::perspectiveDivide(const ClipVertex& vertex) const
{
    if (vertex.w <= 0.0f) throw std::runtime_error("Vertex is behind camera");

    ScreenVertex result;
    // 透视除法后转成以屏幕中心为原点的像素单位。
    result.x = (vertex.x / vertex.w) * (width * 0.5f);
    result.y = (vertex.y / vertex.w) * (height * 0.5f);
    // 光栅器用相机空间 z 做透视校正插值和深度比较。
    result.z = vertex.viewZ;
    result.u = vertex.u;
    result.v = vertex.v;

    return result;
}
