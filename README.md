# MyRasterizer

一个使用 C++17 编写的软件光栅化学习项目。程序将两个三角形光栅化到 800 × 600 的图像中，并输出为 PPM 图片。

## 当前功能

- 使用重心坐标判断像素是否位于三角形内
- 插值三角形顶点的深度和纹理坐标
- 使用 Z 缓冲进行深度测试（深度值越小越靠近相机）
- 对纹理进行采样，并将结果写入 ASCII PPM（P3）格式的 `triangle.ppm`

目前纹理由程序创建，所有纹理像素使用同一种颜色；尚未实现从图片文件加载纹理。

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

程序会在当前工作目录生成 `triangle.ppm`。之后修改代码时，重新构建并运行：

```bash
cmake --build build && ./build/renderStudy
```

## 直接编译

也可以不使用 CMake，直接编译所有源文件：

```bash
c++ -std=c++17 -Iinclude src/main.cpp src/Texture.cpp -o renderStudy
./renderStudy
```

## VS Code

CMake 配置会生成 `build/compile_commands.json`，供 C/C++ 扩展的 IntelliSense 使用。首次打开项目或修改 CMake 配置后，先运行：

```bash
cmake -S . -B build
```

## 项目结构

```text
.
├── .vscode/
│   └── c_cpp_properties.json
├── CMakeLists.txt
├── include/
│   ├── Color.h
│   └── Texture.h
└── src/
    ├── main.cpp
    └── Texture.cpp
```
