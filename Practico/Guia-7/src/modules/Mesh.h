#pragma once
#include <glm/glm.hpp>
#include <vector>
// Los tres atributos permanecen juntos en cada vertice.
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
class Mesh {
public:
    Mesh() = default;
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    void load(const MeshData& data);
    void clear();
    unsigned int vao() const { return vao_; }
    int count() const { return count_; }
private:
    unsigned int vao_ = 0, vbo_ = 0, ebo_ = 0;
    int count_ = 0;
};
