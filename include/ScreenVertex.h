
#pragma once

struct ScreenVertex {
    // 屏幕坐标（暂时使用浮点数）
    float x;
    float y;

    // 保留摄像机空间深度
    float z;

    // 纹理坐标
    float u;
    float v;
};
