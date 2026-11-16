# MyRasterizer

一个使用 C++ 实现的软件光栅化学习项目。当前程序将两个三角形光栅化到 800 × 600 的图像中，并将结果写入 PPM 文件。

## 功能

- 使用重心坐标判断像素是否位于三角形内
- 对三角形顶点的颜色和深度进行插值
- 使用 Z 缓冲进行深度测试（深度值越小越靠近相机）
- 输出 ASCII PPM（P3）格式的 `triangle.ppm`

## 环境要求

- CMake 3.16 或更高版本
- 支持 C++17 的编译器

## 使用 CMake 构建和运行

在项目根目录首次配置并构建：

```bash
cmake -S . -B build
cmake --build build
```

运行程序：

```bash
./build/renderStudy
```

程序会在当前工作目录生成 `triangle.ppm`。之后修改代码时，重新构建并运行即可：

```bash
cmake --build build && ./build/renderStudy
```

## 直接编译

也可以不使用 CMake，直接用 C++ 编译器构建入口文件：

```bash
c++ -std=c++17 src/main.cpp -o renderStudy
./renderStudy
```

## 项目结构

```text
.
├── CMakeLists.txt
└── src/
    └── main.cpp
```
