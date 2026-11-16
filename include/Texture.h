#pragma once

#include <vector>
#include <string>
#include "Color.h"


class Texture {
public:
    int width;
    int height;

    std::vector<Color> pixels;

    Texture()=default;


    Texture(int width, int height);

     // 从图片文件加载纹理
    bool loadFromFile(const std::string& path);

    // 根据 UV 坐标采样纹理颜色
    Color sample(float u, float v) const;
};