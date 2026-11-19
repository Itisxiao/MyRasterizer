#pragma once

#include "Vertex.h"
#include <string>
#include <vector>

struct ObjMesh {
    // OBJ 面已三角化，按 a,b,c,a,b,c 顺序存储。
    std::vector<Vertex> triangles;
};

bool loadObj(const std::string& path, ObjMesh& mesh, std::string& error);
