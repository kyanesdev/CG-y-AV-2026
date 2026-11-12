#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>

struct RenderItem {
    const Mesh* mesh = nullptr; // No es propietario de la malla.
    glm::mat4 model{1.0f};
    glm::vec3 color{1.0f};
};

class Aircraft {
public:
    void init();
    void update(const glm::vec3& position, const glm::vec3& angles);
    void collect(std::vector<RenderItem>& items) const;
    std::size_t piece_count() const { return parts_.size(); }
    std::size_t mesh_count() const { return meshes_.size(); }
private:
    struct Part {
        std::size_t mesh_index;
        glm::mat4 local{1.0f};
        glm::vec3 color{1.0f};
    };
    std::vector<Mesh> meshes_;
    std::vector<Part> parts_;
    glm::mat4 pose_{1.0f};
    // Punto de referencia cerca de la zona alar; dimensiones normalizadas.
    const glm::vec3 reference_{-0.10f, 0.0f, 0.0f};
};
