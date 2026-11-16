
#include "Projection.h"
#include <cmath>
#include <stdexcept>

Projection::Projection(float width, float height, float fov)
    : width(width), height(height), fov(fov)
{
    const float pi = 3.14159265358979323846f;

    // 根据垂直视场角计算焦距
    focalLength = (height / 2.0f) /
                  std::tan(fov * pi / 360.0f);
}

ScreenVertex Projection::project(const Vertex& vertex) const
{
    if (vertex.z <= 0.0f) {
        throw std::runtime_error("Vertex is behind camera");
    }

    ScreenVertex result;

    // 透视投影
    result.x = focalLength * vertex.x / vertex.z;
    result.y = focalLength * vertex.y / vertex.z;

    result.z = vertex.z;

    // 保留纹理坐标
    result.u = vertex.u;
    result.v = vertex.v;

    return result;
}
