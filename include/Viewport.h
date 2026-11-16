
#pragma once

#include "ScreenVertex.h"

class Viewport {
public:
    Viewport(float width, float height);

    ScreenVertex transform(const ScreenVertex& vertex) const;

private:
    float width;
    float height;
};
