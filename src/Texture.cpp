#include "Texture.h"
#include <algorithm>

Texture::Texture(int width, int height)
    : width(width), height(height) {
    pixels.resize(width * height, Color(233, 23, 1));
}

Color Texture::sample(float u, float v) const {

    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (width - 1));
    int y = static_cast<int>(v * (height - 1));

    return pixels[y * width + x];
}