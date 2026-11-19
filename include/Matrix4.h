#pragma once

struct Vector4 {
    float x, y, z, w;
};

struct Matrix4 {
    float m[4][4]{};

    static Matrix4 identity();
    static Matrix4 rotationX(float radians);
    static Matrix4 rotationY(float radians);
    static Matrix4 translation(float x, float y, float z);
    static Matrix4 scale(float scaleX, float scaleY, float scaleZ);

    Vector4 operator*(const Vector4& v) const;
    Matrix4 operator*(const Matrix4& other) const;
};