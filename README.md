# MyRasterizer

一个使用 C++17 编写的软件光栅化学习项目。程序将纹理映射到经过旋转的长方体上，通过透视投影和深度测试生成 PPM 图片。

## 当前功能

- 将长方体拆分为三角形面片，并使用重心坐标进行光栅化
- 将长方体绕 X、Y 轴旋转，再进行透视投影
- 使用透视校正插值纹理坐标和深度
- 使用 Z 缓冲进行深度测试（深度值越小越靠近相机）
- 对不同朝向的面应用亮度差异，便于观察长方体结构
- 使用捆绑的 `stb_image` 从 `assets/test.png` 加载纹理
- 将渲染结果写入 ASCII PPM（P3）格式的 `triangle.ppm`

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

请从项目根目录运行程序，这样它才能找到 `assets/test.png`。

## 直接编译

也可以不使用 CMake，直接编译所有源文件：

```bash
c++ -std=c++17 -Iinclude -Ithird_party \
  src/main.cpp src/Texture.cpp src/Projection.cpp src/Viewport.cpp \
  -o renderStudy
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
├── assets/
│   └── test.png
├── include/
│   ├── Color.h
│   ├── Projection.h
│   ├── ScreenVertex.h
│   ├── Texture.h
│   ├── Vertex.h
│   └── Viewport.h
├── src/
│   ├── main.cpp
│   ├── Projection.cpp
│   ├── Texture.cpp
│   └── Viewport.cpp
└── third_party/
    └── stb_image.h
```
