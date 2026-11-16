#pragma once

#include <vector>
#include "Color.h"

class Texture {
public:
    int width;
    int height;

    std::vector<Color> pixels;

    Texture(int width, int height);

    Color sample(float u, float v) const;
};