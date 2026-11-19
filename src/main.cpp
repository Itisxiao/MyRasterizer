
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
#include "Matrix4.h"
#include "ObjLoader.h"


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

Vertex transformVertex(
    const Vertex& vertex,
    const Matrix4& modelViewMatrix
) {
    const Vector4 transformed = modelViewMatrix * Vector4{
        vertex.x, vertex.y, vertex.z, 1.0f
    };

    return {
        transformed.x,
        transformed.y,
        transformed.z,
        vertex.u,
        vertex.v
    };
}

bool isFrontFacingInViewSpace(const Vertex& a, const Vertex& b, const Vertex& c)
{
    const float abX = b.x - a.x;
    const float abY = b.y - a.y;
    const float abZ = b.z - a.z;
    const float acX = c.x - a.x;
    const float acY = c.y - a.y;
    const float acZ = c.z - a.z;

    const float normalX = abY * acZ - abZ * acY;
    const float normalY = abZ * acX - abX * acZ;
    const float normalZ = abX * acY - abY * acX;
    const float centerX = (a.x + b.x + c.x) / 3.0f;
    const float centerY = (a.y + b.y + c.y) / 3.0f;
    const float centerZ = (a.z + b.z + c.z) / 3.0f;

    // View Space 相机在原点；若法线朝向相机，点积应为正。
    return normalX * -centerX + normalY * -centerY + normalZ * -centerZ > 0.0f;
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
    const Texture& texture,
    float shade
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
            const float reciprocalZ =
                alpha / a.z +
                beta  / b.z +
                gamma / c.z;
            const float z = 1.0f / reciprocalZ;

            int index = y * width + x;

            // 2. 深度测试
            if (z < zBuffer[index]) {

                // 3. 更新深度
                zBuffer[index] = z;

                // 4. 插值得到当前像素的纹理坐标
                float u = (
                    alpha * a.u / a.z +
                    beta  * b.u / b.z +
                    gamma * c.u / c.z
                ) / reciprocalZ;

                float v = (
                    alpha * a.v / a.z +
                    beta  * b.v / b.z +
                    gamma * c.v / c.z
                ) / reciprocalZ;

                // 5. 采样纹理颜色        
                Color color = texture.sample(u, v);

                // 6. 对不同面应用亮度差异，便于区分长方体的朝向
                pixels[index * 3 + 0] = std::clamp(
                    static_cast<int>(color.r * shade), 0, 255);
                pixels[index * 3 + 1] = std::clamp(
                    static_cast<int>(color.g * shade), 0, 255);
                pixels[index * 3 + 2] = std::clamp(
                    static_cast<int>(color.b * shade), 0, 255);
            }
        }
    }
}
constexpr float NEAR_PLANE = 1.0f;
constexpr float FAR_PLANE = 100.0f;

float clipDistance(const ClipVertex& v, int plane) {
    switch (plane) {
        case 0: return v.x + v.w; // left:   x >= -w
        case 1: return v.w - v.x; // right:  x <=  w
        case 2: return v.y + v.w; // bottom: y >= -w
        case 3: return v.w - v.y; // top:    y <=  w
        case 4: return v.z + v.w; // near:   z >= -w
        default:return v.w - v.z; // far:    z <=  w
    }
}

ClipVertex interpolateClipVertex(const ClipVertex& a, const ClipVertex& b, float t) {
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t,
        a.viewZ + (b.viewZ - a.viewZ) * t,
        a.u + (b.u - a.u) * t,
        a.v + (b.v - a.v) * t
    };
}

std::vector<ClipVertex> clipPolygonHomogeneous(std::vector<ClipVertex> polygon) {
    for (int plane = 0; plane < 6 && !polygon.empty(); ++plane) {
        std::vector<ClipVertex> output;
        for (size_t i = 0; i < polygon.size(); ++i) {
            const ClipVertex& a = polygon[i];
            const ClipVertex& b = polygon[(i + 1) % polygon.size()];
            const float distanceA = clipDistance(a, plane);
            const float distanceB = clipDistance(b, plane);
            const bool insideA = distanceA >= 0.0f;
            const bool insideB = distanceB >= 0.0f;

            if (insideA && insideB) output.push_back(b);
            else if (insideA && !insideB) {
                const float t = distanceA / (distanceA - distanceB);
                output.push_back(interpolateClipVertex(a, b, t));
            } else if (!insideA && insideB) {
                const float t = distanceA / (distanceA - distanceB);
                output.push_back(interpolateClipVertex(a, b, t));
                output.push_back(b);
            }
        }
        polygon = std::move(output);
    }
    return polygon;
}

int main() {
    const float width = 800.0f;
    const float height = 600.0f;

    // 创建投影器：垂直视场角 60 度
    Projection projection(width, height, 60.0f, NEAR_PLANE, FAR_PLANE);
    const float aspectRatio = width / height;
    const float fovRadians = 60.0f * 3.14159265358979323846f / 180.0f;
    const Matrix4 projectionMatrix = Matrix4::perspective(
        fovRadians, aspectRatio, NEAR_PLANE, FAR_PLANE
    );

    // 创建视口
    Viewport viewport(width, height);

    // int textureWidth = 256;
    // int textureHeight = 256;

    //Texture texture(textureWidth, textureHeight);
    Texture texture;
     if (!texture.loadFromFile("assets/test.png")) {
        return -1;
    }

    ObjMesh mesh;
    std::string objError;
    if (!loadObj("assets/model.obj", mesh, objError)) {
        std::cerr << "OBJ load failed: " << objError << std::endl;
        return 1;
    }

    

    // 创建像素缓冲区，初始化为黑色
    std::vector<int> pixels(width * height * 3, 0);

    // 创建深度缓冲区，初始化为最大值
    std::vector<float> zBuffer(width * height, std::numeric_limits<float>::max());

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const Vertex& vertex : mesh.triangles) {
        minX = std::min(minX, vertex.x); maxX = std::max(maxX, vertex.x);
        minY = std::min(minY, vertex.y); maxY = std::max(maxY, vertex.y);
        minZ = std::min(minZ, vertex.z); maxZ = std::max(maxZ, vertex.z);
    }
    const float centerX = (minX + maxX) * 0.5f;
    const float centerY = (minY + maxY) * 0.5f;
    const float centerZ = (minZ + maxZ) * 0.5f;
    const float maxExtent = std::max({maxX - minX, maxY - minY, maxZ - minZ});
    if (maxExtent <= 1e-6f) {
        std::cerr << "OBJ model has zero size" << std::endl;
        return 1;
    }
    for (Vertex& vertex : mesh.triangles) {
        vertex.x = (vertex.x - centerX) / maxExtent * 2.0f;
        vertex.y = (vertex.y - centerY) / maxExtent * 2.0f;
        vertex.z = (vertex.z - centerZ) / maxExtent * 2.0f;
    }
    constexpr float rotationX = 0.35f;
    constexpr float rotationY = 0.65f;
    // 列向量约定下，右侧变换先作用：先旋转，再平移。
    const Matrix4 modelMatrix =
        Matrix4::translation(0.0f, 0.0f, 12.0f)
        * Matrix4::rotationY(rotationY)
        * Matrix4::rotationX(rotationX)
        * Matrix4::scale(1.5f, 1.5f, 1.5f);
    const Matrix4 viewMatrix = Matrix4::lookAt(
        {0.0f, 0.0f, 0.0f, 1.0f}, // 摄像机位置
        {0.0f, 0.0f, 1.0f, 1.0f}, // 观察目标：朝 +Z
        {0.0f, 1.0f, 0.0f, 0.0f}  // 世界上方向
    );
    const Matrix4 modelViewMatrix = viewMatrix * modelMatrix;

    // 旋转并放置长方体，然后对每个四边形面进行透视投影和光栅化。
    // for (int face = 0; face < 6; ++face) {
    //     ScreenVertex screenVertices[4];
    //     for (int i = 0; i < 4; ++i) {
    //         Vertex worldVertex = rotateAndTranslate(
    //             faces[face][i],
    //             rotationX,
    //             rotationY,
    //             0.0f,
    //             0.0f,
    //             12.0f
    //         );
    //         screenVertices[i] = viewport.transform(
    //             projection.project(worldVertex)
    //         );
    //     }

    //     drawTriangle(
    //         screenVertices[0], screenVertices[1], screenVertices[2],
    //         width, height, pixels, zBuffer, texture, faceShades[face]
    //     );
    //     drawTriangle(
    //         screenVertices[0], screenVertices[2], screenVertices[3],
    //         width, height, pixels, zBuffer, texture, faceShades[face]
    //     );
    // }

    auto renderClippedTriangle = [&](const Vertex& a, const Vertex& b, const Vertex& c,
                                float shade) {
        // 先变换到摄像机空间，再裁剪
        std::vector<Vertex> triangle = {
            transformVertex(a, modelViewMatrix),
            transformVertex(b, modelViewMatrix),
            transformVertex(c, modelViewMatrix)
        };

        // Homogeneous clipping 在透视除法之前对完整视锥进行裁剪。
        std::vector<ClipVertex> clipPolygon;
        clipPolygon.reserve(triangle.size());
        for (const Vertex& v : triangle) {
            clipPolygon.push_back(projection.toClipSpace(v, projectionMatrix));
        }
        std::vector<ClipVertex> clipped = clipPolygonHomogeneous(std::move(clipPolygon));
        if (clipped.size() < 3) return;

        std::vector<ScreenVertex> screen;
        screen.reserve(clipped.size());
        for (const ClipVertex& v : clipped) {
            screen.push_back(viewport.transform(projection.perspectiveDivide(v)));
        }

        // 裁剪结果是凸多边形，以第 0 个顶点为扇心三角化
        for (size_t i = 1; i + 1 < screen.size(); ++i) {
            // 在 View Space 判断朝向，跳过背向相机的三角形。
            // 维持 Back-face Culling 在 View Space 进行。
            if (!isFrontFacingInViewSpace(triangle[0], triangle[1], triangle[2])) {
                continue;
            }
            drawTriangle(
                screen[0], screen[i], screen[i + 1],
                width, height, pixels, zBuffer, texture, shade
            );
        }
    };

    for (size_t i = 0; i + 2 < mesh.triangles.size(); i += 3) {
        renderClippedTriangle(
            mesh.triangles[i], mesh.triangles[i + 1], mesh.triangles[i + 2], 1.0f
        );
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
