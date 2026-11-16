
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include "Texture.h"
#include "Vertex.h"
#include "Projection.h"
#include "Viewport.h"


struct Point {
    float x;
    float y;
};



// 计算三角形的有向面积（叉积的两倍）
float edgeFunction(
    Point a, 
    Point b, 
    Point p
) {
    return (p.x - a.x) * (b.y - a.y)
         - (p.y - a.y) * (b.x - a.x);
}

// 判断点是否在三角形内部
bool insideTriangle(
    Point p,
    Point a,
    Point b,
    Point c
) {
    float w1 = edgeFunction(a, b, p);
    float w2 = edgeFunction(b, c, p);
    float w3 = edgeFunction(c, a, p);

    // 三个边的叉积符号一致，说明在三角形内
    bool hasNegative = (w1 < 0 || w2 < 0 || w3 < 0);
    bool hasPositive = (w1 > 0 || w2 > 0 || w3 > 0);

    return !(hasNegative && hasPositive);
}

// 计算重心坐标
bool barycentric(
    Point p,
    Point a,
    Point b,
    Point c,
    float& alpha,
    float& beta,
    float& gamma
) {
    float area = edgeFunction(a, b, c);

    if (std::abs(area) < 1e-6f) {
        return false;
    }

    //α 越大，P 越靠近 A。
    // β 越大，P 越靠近 B。
    // γ 越大，P 越靠近 C。
    //面积比值法计算重心坐标
    alpha = edgeFunction(b, c, p) / area;
    beta  = edgeFunction(c, a, p) / area;
    gamma = edgeFunction(a, b, p) / area;

    return alpha >= 0 && beta >= 0 && gamma >= 0;
}

Vertex rotateYAndTranslate(
    const Vertex& vertex,
    float angle,
    float translateX,
    float translateY,
    float translateZ
) {
    const float cosAngle = std::cos(angle);
    const float sinAngle = std::sin(angle);

    return {
        cosAngle * vertex.x + sinAngle * vertex.z + translateX,
        vertex.y + translateY,
        -sinAngle * vertex.x + cosAngle * vertex.z + translateZ,
        vertex.u,
        vertex.v
    };
}

void drawTriangle(
    const Vertex& a,
    const Vertex& b,
    const Vertex& c,
    int width,
    int height,
    std::vector<int>& pixels,
    std::vector<float>& zBuffer,
    const Texture& texture
) {
    int minX = std::max(0, (int)std::floor(
        std::min({a.x, b.x, c.x})));

    int maxX = std::min(width - 1, (int)std::ceil(
        std::max({a.x, b.x, c.x})));

    int minY = std::max(0, (int)std::floor(
        std::min({a.y, b.y, c.y})));

    int maxY = std::min(height - 1, (int)std::ceil(
        std::max({a.y, b.y, c.y})));

    float area = edgeFunction(
        {a.x, a.y},
        {b.x, b.y},
        {c.x, c.y}
    );

    if (std::abs(area) < 1e-6f) {
        return;
    }

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            Point p = {
                x + 0.5f,
                y + 0.5f
            };

            float alpha, beta, gamma;

            if (!barycentric(
                p,
                {a.x, a.y},
                {b.x, b.y},
                {c.x, c.y},
                alpha, beta, gamma
            )) {
                continue;
            }

            // 1. 插值得到当前像素的深度
            float z =
                alpha * a.z +
                beta  * b.z +
                gamma * c.z;

            int index = y * width + x;

            // 2. 深度测试
            if (z < zBuffer[index]) {

                // 3. 更新深度
                zBuffer[index] = z;

                // 4. 插值得到当前像素的纹理坐标
                float u = alpha * a.u
                        + beta  * b.u
                        + gamma * c.u;

                float v = alpha * a.v
                        + beta  * b.v
                        + gamma * c.v;

                // 5. 采样纹理颜色        
                 Color color = texture.sample(u, v);

                //6.插值颜色更新颜色缓冲区
                pixels[index * 3 + 0] = color.r;
                pixels[index * 3 + 1] = color.g;
                pixels[index * 3 + 2] = color.b;
            }
        }
    }
}

void drawTriangle(
    const ScreenVertex& a,
    const ScreenVertex& b,
    const ScreenVertex& c,
    int width,
    int height,
    std::vector<int>& pixels,
    std::vector<float>& zBuffer,
    const Texture& texture
) {
    int minX = std::max(0, (int)std::floor(
        std::min({a.x, b.x, c.x})));

    int maxX = std::min(width - 1, (int)std::ceil(
        std::max({a.x, b.x, c.x})));

    int minY = std::max(0, (int)std::floor(
        std::min({a.y, b.y, c.y})));

    int maxY = std::min(height - 1, (int)std::ceil(
        std::max({a.y, b.y, c.y})));

    float area = edgeFunction(
        {a.x, a.y},
        {b.x, b.y},
        {c.x, c.y}
    );

    if (std::abs(area) < 1e-6f) {
        return;
    }

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            Point p = {
                x + 0.5f,
                y + 0.5f
            };

            float alpha, beta, gamma;

            if (!barycentric(
                p,
                {a.x, a.y},
                {b.x, b.y},
                {c.x, c.y},
                alpha, beta, gamma
            )) {
                continue;
            }

            // 1. 插值得到当前像素的深度
            float z =
                alpha * a.z +
                beta  * b.z +
                gamma * c.z;

            int index = y * width + x;

            // 2. 深度测试
            if (z < zBuffer[index]) {

                // 3. 更新深度
                zBuffer[index] = z;

                // 4. 插值得到当前像素的纹理坐标
                float u = alpha * a.u
                        + beta  * b.u
                        + gamma * c.u;

                float v = alpha * a.v
                        + beta  * b.v
                        + gamma * c.v;

                // 5. 采样纹理颜色        
                 Color color = texture.sample(u, v);

                //6.插值颜色更新颜色缓冲区
                pixels[index * 3 + 0] = color.r;
                pixels[index * 3 + 1] = color.g;
                pixels[index * 3 + 2] = color.b;
            }
        }
    }
}

int main() {
    const float width = 800.0f;
    const float height = 600.0f;

    // 创建投影器：垂直视场角 60 度
    Projection projection(width, height, 60.0f);

    // 创建视口
    Viewport viewport(width, height);

    // int textureWidth = 256;
    // int textureHeight = 256;

    //Texture texture(textureWidth, textureHeight);
    Texture texture;
     if (!texture.loadFromFile("assets/test.png")) {
        return -1;
    }

    

    // 创建像素缓冲区，初始化为黑色
    std::vector<int> pixels(width * height * 3, 0);

    // 创建深度缓冲区，初始化为最大值
    std::vector<float> zBuffer(width * height, std::numeric_limits<float>::max());

    const Vertex localTriangle[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f, 0.5f, 1.0f}
    };

    const float rotationY = 0.55f;
    Vertex nearTriangle[3];
    Vertex farTriangle[3];

    for (int i = 0; i < 3; ++i) {
        nearTriangle[i] = rotateYAndTranslate(
            localTriangle[i], rotationY, -1.3f, 0.0f, 3.2f
        );
        farTriangle[i] = rotateYAndTranslate(
            localTriangle[i], rotationY, 2.1f, 0.0f, 5.4f
        );
    }

    // 1. 三维顶点 -> 透视投影
    auto p0 = projection.project(nearTriangle[0]);
    auto p1 = projection.project(nearTriangle[1]);
    auto p2 = projection.project(nearTriangle[2]);

    auto p3 = projection.project(farTriangle[0]);
    auto p4 = projection.project(farTriangle[1]);
    auto p5 = projection.project(farTriangle[2]);

    // 2. 投影坐标 -> 屏幕像素坐标
    auto s0 = viewport.transform(p0);
    auto s1 = viewport.transform(p1);
    auto s2 = viewport.transform(p2);
    
    auto s3 = viewport.transform(p3);
    auto s4 = viewport.transform(p4);
    auto s5 = viewport.transform(p5);


    // 3. 交给原来的光栅化器
    drawTriangle(s0, s1, s2, width, height, pixels, zBuffer, texture);
    drawTriangle(s3, s4, s5, width, height, pixels, zBuffer, texture);

   
    //交换结果一样
    //drawTriangle(a2, b2, c2, width, height, pixels, zBuffer);
    //drawTriangle(a1, b1, c1, width, height, pixels, zBuffer);

    // 输出 PPM 图片
    std::ofstream file("triangle.ppm");

    if (!file.is_open()) {
        std::cerr << "Failed to create image!" << std::endl;
        return 1;
    }

    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int index = (y * width + x) * 3;

            file << pixels[index] << " "
                 << pixels[index + 1] << " "
                 << pixels[index + 2] << "\n";
        }
    }

    file.close();

    std::cout << "Image generated: triangle.ppm" << std::endl;

    return 0;
}
