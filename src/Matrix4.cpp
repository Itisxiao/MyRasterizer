
#include "Matrix4.h"
#include <cmath>


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
