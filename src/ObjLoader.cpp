#include "ObjLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
struct Vec3 { float x, y, z; };
struct Vec2 { float u, v; };
struct FaceIndex { int position = 0; int texcoord = 0; int normal = 0; };

int resolveIndex(int index, size_t count, const char* kind, size_t lineNumber)
{
    if (index == 0) {
        throw std::runtime_error("line " + std::to_string(lineNumber) +
                                 ": OBJ indices are 1-based; zero is invalid");
    }
    const long resolved = index > 0 ? static_cast<long>(index) - 1
                                    : static_cast<long>(count) + index;
    if (resolved < 0 || resolved >= static_cast<long>(count)) {
        throw std::runtime_error("line " + std::to_string(lineNumber) +
                                 ": " + kind + " index is out of range");
    }
    return static_cast<int>(resolved);
}

FaceIndex parseFaceIndex(const std::string& token, size_t positionCount,
                         size_t texcoordCount, size_t normalCount, size_t lineNumber)
{
    FaceIndex result;
    const size_t firstSlash = token.find('/');
    const size_t secondSlash = firstSlash == std::string::npos
        ? std::string::npos : token.find('/', firstSlash + 1);

    try {
        result.position = resolveIndex(std::stoi(token.substr(0, firstSlash)),
                                       positionCount, "position", lineNumber);
        if (firstSlash != std::string::npos && secondSlash != firstSlash + 1) {
            const std::string tex = token.substr(firstSlash + 1,
                secondSlash == std::string::npos ? std::string::npos
                                                 : secondSlash - firstSlash - 1);
            if (!tex.empty()) result.texcoord = resolveIndex(std::stoi(tex), texcoordCount,
                                                               "texture", lineNumber) + 1;
        }
        if (secondSlash != std::string::npos && secondSlash + 1 < token.size()) {
            result.normal = resolveIndex(std::stoi(token.substr(secondSlash + 1)),
                                         normalCount, "normal", lineNumber) + 1;
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("line " + std::to_string(lineNumber) +
                                 ": malformed face index '" + token + "'");
    } catch (const std::out_of_range&) {
        throw std::runtime_error("line " + std::to_string(lineNumber) +
                                 ": face index is too large");
    }
    return result;
}
}

bool loadObj(const std::string& path, ObjMesh& mesh, std::string& error)
{
    std::ifstream file(path);
    if (!file) {
        error = "cannot open '" + path + "'";
        return false;
    }

    std::vector<Vec3> positions;
    std::vector<Vec2> texcoords;
    std::vector<Vec3> normals;
    ObjMesh loaded;
    std::string line;
    size_t lineNumber = 0;

    try {
        while (std::getline(file, line)) {
            ++lineNumber;
            std::istringstream row(line);
            std::string tag;
            row >> tag;
            if (tag.empty() || tag[0] == '#') continue;

            if (tag == "v") {
                Vec3 value{};
                if (!(row >> value.x >> value.y >> value.z))
                    throw std::runtime_error("line " + std::to_string(lineNumber) +
                                             ": malformed vertex position");
                positions.push_back(value);
            } else if (tag == "vt") {
                Vec2 value{};
                if (!(row >> value.u >> value.v))
                    throw std::runtime_error("line " + std::to_string(lineNumber) +
                                             ": malformed texture coordinate");
                texcoords.push_back(value);
            } else if (tag == "vn") {
                Vec3 value{};
                if (!(row >> value.x >> value.y >> value.z))
                    throw std::runtime_error("line " + std::to_string(lineNumber) +
                                             ": malformed normal");
                normals.push_back(value);
            } else if (tag == "f") {
                std::vector<FaceIndex> face;
                std::string token;
                while (row >> token) {
                    if (!token.empty() && token[0] == '#') break;
                    face.push_back(parseFaceIndex(token, positions.size(), texcoords.size(),
                                                  normals.size(), lineNumber));
                }
                if (face.size() < 3)
                    throw std::runtime_error("line " + std::to_string(lineNumber) +
                                             ": face must have at least 3 vertices");

                // Fan triangulation supports triangles, quads, and convex n-gons.
                for (size_t i = 1; i + 1 < face.size(); ++i) {
                    const FaceIndex indices[] = {face[0], face[i], face[i + 1]};
                    for (const FaceIndex& index : indices) {
                        const Vec3& p = positions[index.position];
                        float u = 0.0f, v = 0.0f;
                        if (index.texcoord != 0) {
                            const Vec2& uv = texcoords[index.texcoord - 1];
                            u = uv.u;
                            v = 1.0f - uv.v; // OBJ 通常以左下为 UV 原点
                        }
                        loaded.triangles.push_back({p.x, p.y, p.z, u, v});
                    }
                }
            }
        }
    } catch (const std::exception& exception) {
        error = exception.what();
        return false;
    }

    if (loaded.triangles.empty()) {
        error = "no renderable faces found in '" + path + "'";
        return false;
    }
    mesh = std::move(loaded);
    error.clear();
    return true;
}
