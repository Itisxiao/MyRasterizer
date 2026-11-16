
#pragma once

#include "Vertex.h"
#include "ScreenVertex.h"

class Projection {
public:
    Projection(float width, float height, float fov);

    ScreenVertex project(const Vertex& vertex) const;

private:
    float width;
    float height;
    float fov;
    float focalLength;
};
