
#pragma once

#include "Vertex.h"
#include "ScreenVertex.h"
#include "Matrix4.h"

class Projection {
public:
    Projection(float width, float height, float fovDegrees,
               float nearPlane, float farPlane);

    ScreenVertex project(const Vertex& vertex, const Matrix4& projectionMatrix) const;

private:
    float width;
    float height;
    float nearPlane;
    float farPlane;
};
