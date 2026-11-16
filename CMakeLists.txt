cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(renderStudy LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(renderStudy src/main.cpp  src/Texture.cpp src/Projection.cpp src/Viewport.cpp)
target_include_directories(renderStudy
    PRIVATE include third_party
)
target_compile_features(renderStudy PRIVATE cxx_std_17)
set_target_properties(renderStudy PROPERTIES CXX_EXTENSIONS OFF)
