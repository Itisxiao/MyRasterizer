
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

struct Point {
    float x;
    float y;
};

struct Color {
    float r, g, b;
};


// 计算三角形的有向面积（叉积的两倍）
float edgeFunction(Point a, Point b, Point p) {
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

    alpha = edgeFunction(b, c, p) / area;
    beta  = edgeFunction(c, a, p) / area;
    gamma = edgeFunction(a, b, p) / area;

    return alpha >= 0 && beta >= 0 && gamma >= 0;
}

int main() {
    const int width = 800;
    const int height = 600;

    // 创建像素缓冲区，初始化为黑色
    std::vector<int> pixels(width * height * 3, 0);

    // 定义三角形的三个顶点
    Point a = {400, 100};
    Point b = {150, 450};
    Point c = {650, 450};

    // 计算三角形包围盒
    int minX = std::max(0, (int)std::floor(
        std::min({a.x, b.x, c.x})
    ));

    int maxX = std::min(width - 1, (int)std::ceil(
        std::max({a.x, b.x, c.x})
    ));

    int minY = std::max(0, (int)std::floor(
        std::min({a.y, b.y, c.y})
    ));

    int maxY = std::min(height - 1, (int)std::ceil(
        std::max({a.y, b.y, c.y})
    ));

    // 光栅化：遍历包围盒内的像素
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            // 使用像素中心点进行判断
            Point p = {
                x + 0.5f,
                y + 0.5f
            };

            Color ca = {255, 0, 0};
            Color cb = {0, 255, 0};
            Color cc = {0, 0, 255};

            float alpha, beta, gamma;

            if (barycentric(p, a, b, c, alpha, beta, gamma)) {

                int index = (y * width + x) * 3;

                pixels[index] = static_cast<int>(
                    alpha * ca.r + beta * cb.r + gamma * cc.r
                );

                pixels[index + 1] = static_cast<int>(
                    alpha * ca.g + beta * cb.g + gamma * cc.g
                );

                pixels[index + 2] = static_cast<int>(
                    alpha * ca.b + beta * cb.b + gamma * cc.b
                );
            }
        }
    }

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
