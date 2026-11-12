#include "Aircraft.h"
#include "Primitives.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <utility>

namespace {
glm::mat4 local_matrix(const glm::vec3& position,
                       const glm::vec3& rotation_deg,
                       const glm::vec3& scale) {
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation_deg.x), glm::vec3(1,0,0));
    m = glm::rotate(m, glm::radians(rotation_deg.y), glm::vec3(0,1,0));
    m = glm::rotate(m, glm::radians(rotation_deg.z), glm::vec3(0,0,1));
    return glm::scale(m, scale);
}
}

void Aircraft::init() {
    if (!meshes_.empty()) throw std::logic_error("Aircraft::init() ya ejecutado");
    // Solo TRES mallas: una de cada primitiva. Se comparten entre piezas.
    meshes_.reserve(3);
    meshes_.emplace_back(); meshes_.back().load(primitives::cube());
    meshes_.emplace_back(); meshes_.back().load(primitives::cylinder(0.5f, 1.0f, 20, 1));
    meshes_.emplace_back(); meshes_.back().load(primitives::cone(0.5f, glm::radians(32.0f), 20, 1));
    parts_.reserve(8);
    auto add = [this](std::size_t index, glm::vec3 pos, glm::vec3 angles,
                      glm::vec3 scale, glm::vec3 color) {
        parts_.push_back({index, local_matrix(pos, angles, scale), color});
    };
    // X es el eje longitudinal, nariz hacia +X; Y arriba, Z transversal.
    // Fuselaje dividido en dos cilindros (eje original Y -> X).
    add(1, {-0.27f,0,0}, {0,0,-90}, {0.31f,1.38f,0.31f}, {0.78f,0.81f,0.88f});
    add(1, {-0.97f,0.02f,0}, {0,0,-90}, {0.22f,0.48f,0.22f}, {0.58f,0.68f,0.81f});
    // Nariz de cono, punta hacia +X.
    add(2, {0.53f,0,0}, {0,0,-90}, {0.42f,0.50f,0.42f}, {0.94f,0.62f,0.23f});
    // Ala izquierda y derecha (sin reflejos ni escalados negativos).
    add(0, {-0.10f,-0.02f,-0.53f}, {0,0,0}, {0.47f,0.055f,1.04f}, {0.22f,0.62f,0.82f});
    add(0, {-0.10f,-0.02f, 0.53f}, {0,0,0}, {0.47f,0.055f,1.04f}, {0.22f,0.62f,0.82f});
    // Estabilizadores horizontales, dos piezas simetricas.
    add(0, {-1.08f,0.05f,-0.27f}, {0,0,0}, {0.29f,0.045f,0.53f}, {0.93f,0.37f,0.32f});
    add(0, {-1.08f,0.05f, 0.27f}, {0,0,0}, {0.29f,0.045f,0.53f}, {0.93f,0.37f,0.32f});
    // Deriva vertical.
    add(0, {-1.06f,0.24f,0}, {0,0,-18}, {0.36f,0.42f,0.055f}, {0.93f,0.37f,0.32f});
}

void Aircraft::update(const glm::vec3& position, const glm::vec3& angles) {
    // Posicion y orientacion expresadas en mundo. Angulos en RADIANES.
    // Pose = T(posicion) * R(angulos) * T(-referencia).
    glm::mat4 m = glm::translate(glm::mat4(1.0f), position);
    m = glm::rotate(m, angles.y, glm::vec3(0,1,0)); // guinada
    m = glm::rotate(m, angles.z, glm::vec3(0,0,1)); // cabeceo (alas // Z)
    m = glm::rotate(m, angles.x, glm::vec3(1,0,0)); // alabeo
    pose_ = glm::translate(m, -reference_);
}

void Aircraft::collect(std::vector<RenderItem>& items) const {
    for (const Part& p : parts_) {
        items.push_back({&meshes_[p.mesh_index], pose_ * p.local, p.color});
    }
}
