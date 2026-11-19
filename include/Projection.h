
#pragma once

#include "Vertex.h"
#include "ScreenVertex.h"
#include "Matrix4.h"

struct ClipVertex {
    float x, y, z, w;
    float viewZ;
    float u, v;
};

class Projection {
public:
    Projection(float width, float height, float fovDegrees,
               float nearPlane, float farPlane);

    ClipVertex toClipSpace(const Vertex& vertex, const Matrix4& projectionMatrix) const;
    ScreenVertex perspectiveDivide(const ClipVertex& vertex) const;

private:
    float width;
    float height;
    float nearPlane;
    float farPlane;
};
