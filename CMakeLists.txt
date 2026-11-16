cmake_minimum_required(VERSION 3.16)

project(renderStudy LANGUAGES CXX)

add_executable(renderStudy src/main.cpp)
target_compile_features(renderStudy PRIVATE cxx_std_17)
set_target_properties(renderStudy PROPERTIES CXX_EXTENSIONS OFF)
