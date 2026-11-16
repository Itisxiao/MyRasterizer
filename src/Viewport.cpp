
#include "Viewport.h"

Viewport::Viewport(float width, float height)
    : width(width), height(height)
{
}

ScreenVertex Viewport::transform(const ScreenVertex& vertex) const
{
    ScreenVertex result = vertex;

    // 从以中心为原点的坐标转换为像素坐标
    result.x = width / 2.0f + vertex.x;
    result.y = height / 2.0f - vertex.y;

    return result;
}
