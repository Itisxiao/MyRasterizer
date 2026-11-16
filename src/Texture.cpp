#include "Texture.h"
#include <algorithm>
#include <iostream>
// 只允许在一个 cpp 文件中定义这个宏
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(int width, int height)
    : width(width), height(height) {
    pixels.resize(width * height, Color(233, 23, 1));
}

bool Texture::loadFromFile(const std::string& path) {

    int channels = 0;

    // 强制解码成 RGB 三通道
    unsigned char* data = stbi_load(
        path.c_str(),
        &width,
        &height,
        &channels,
        3
    );

    if (data == nullptr) {
        std::cerr << "Failed to load texture: "
                  << path << std::endl;

        return false;
    }

    // 分配像素空间
    pixels.resize(width * height);

    // 将解码后的 RGB 数据复制到自己的 Color 数组
    for (int i = 0; i < width * height; i++) {

        pixels[i] = Color(
            data[i * 3 + 0],
            data[i * 3 + 1],
            data[i * 3 + 2]
        );
    }

    // 释放 stb_image 分配的内存
    stbi_image_free(data);

    std::cout << "Texture loaded: "
              << width << "x" << height
              << std::endl;

    return true;
}

Color Texture::sample(float u, float v) const {

    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (width - 1));
    int y = static_cast<int>(v * (height - 1));

    return pixels[y * width + x];
}