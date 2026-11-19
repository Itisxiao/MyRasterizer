
#include "Matrix4.h"
#include <cmath>
#include <stdexcept>

namespace {
Vector4 normalize3(const Vector4& v)
{
    const float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length < 1e-6f) throw std::invalid_argument("lookAt direction must be non-zero");
    return {v.x / length, v.y / length, v.z / length, 0.0f};
}

Vector4 cross3(const Vector4& a, const Vector4& b)
{
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x, 0.0f};
}

float dot3(const Vector4& a, const Vector4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
}


Matrix4 Matrix4::identity()
{
    Matrix4 result{};
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

Matrix4 Matrix4::rotationX(float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);

    Matrix4 result = identity();
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    return result;
}

Matrix4 Matrix4::rotationY(float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);

    Matrix4 result = identity();
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    return result;
}

Matrix4 Matrix4::translation(float x, float y, float z)
{
    Matrix4 result = identity();
    result.m[0][3] = x;
    result.m[1][3] = y;
    result.m[2][3] = z;
    return result;
}

Matrix4 Matrix4::scale(float scaleX, float scaleY, float scaleZ)
{
    Matrix4 result = identity();
    result.m[0][0] = scaleX;
    result.m[1][1] = scaleY;
    result.m[2][2] = scaleZ;
    return result;
}

Matrix4 Matrix4::perspective(float verticalFovRadians, float aspectRatio,
                             float nearPlane, float farPlane)
{
    if (verticalFovRadians <= 0.0f || verticalFovRadians >= 3.14159265f ||
        aspectRatio <= 0.0f || nearPlane <= 0.0f || farPlane <= nearPlane) {
        throw std::invalid_argument("Invalid perspective projection parameters");
    }

    const float focal = 1.0f / std::tan(verticalFovRadians * 0.5f);
    Matrix4 result{};
    result.m[0][0] = focal / aspectRatio;
    result.m[1][1] = focal;
    // Right-handed algebra with +Z forward: NDC z maps near->-1 and far->+1.
    result.m[2][2] = (farPlane + nearPlane) / (farPlane - nearPlane);
    result.m[2][3] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    result.m[3][2] = 1.0f;
    return result;
}

Matrix4 Matrix4::lookAt(const Vector4& eye, const Vector4& target, const Vector4& up)
{
    const Vector4 forward = normalize3({target.x - eye.x, target.y - eye.y,
                                        target.z - eye.z, 0.0f});
    const Vector4 right = normalize3(cross3(up, forward));
    const Vector4 cameraUp = cross3(forward, right);

    Matrix4 result = identity();
    result.m[0][0] = right.x;    result.m[0][1] = right.y;    result.m[0][2] = right.z;
    result.m[1][0] = cameraUp.x; result.m[1][1] = cameraUp.y; result.m[1][2] = cameraUp.z;
    result.m[2][0] = forward.x;  result.m[2][1] = forward.y;  result.m[2][2] = forward.z;
    result.m[0][3] = -dot3(right, eye);
    result.m[1][3] = -dot3(cameraUp, eye);
    result.m[2][3] = -dot3(forward, eye);
    return result;
}

//列向量约定：矩阵乘向量时，矩阵放左边
Vector4 Matrix4::operator*(const Vector4& v) const
{
    return {
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
        m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
    };
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    Matrix4 result{};

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            for (int k = 0; k < 4; ++k) {
                result.m[row][col] += m[row][k] * other.m[k][col];
            }
        }
    }

    return result;
}
