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
    parts_.reserve(90);
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

    // Cabina sencilla, construida con LAS MISMAS primitivas de la catedra.
    // Tablero, lateral de cabina, tres esferas/instrumentos y marco frontal.
    add(0, {0.51f,0.18f,0}, {0,0,0}, {0.18f,0.17f,0.64f}, {0.11f,0.15f,0.19f});
    add(0, {0.10f,0.30f,-0.32f}, {0,0,0}, {0.82f,0.10f,0.045f}, {0.38f,0.43f,0.48f});
    add(0, {0.10f,0.30f, 0.32f}, {0,0,0}, {0.82f,0.10f,0.045f}, {0.38f,0.43f,0.48f});
    add(0, {0.48f,0.58f,0}, {0,0,0}, {0.035f,0.035f,0.73f}, {0.41f,0.46f,0.49f});
    add(0, {0.46f,0.43f,-0.30f}, {0,0,-28}, {0.035f,0.36f,0.035f}, {0.41f,0.46f,0.49f});
    add(0, {0.46f,0.43f, 0.30f}, {0,0,-28}, {0.035f,0.36f,0.035f}, {0.41f,0.46f,0.49f});
    for (float z : {-0.20f,0.0f,0.20f}) {
        add(1, {0.615f,0.19f,z}, {0,0,-90}, {0.17f,0.02f,0.17f}, {0.04f,0.07f,0.09f});
        add(0, {0.630f,0.19f,z}, {0,0,0}, {0.008f,0.005f,0.10f}, {0.85f,0.75f,0.35f});
    }
    cockpit_end_ = parts_.size();

    // Detalles exteriores adicionales: siguen siendo las tres mallas originales.
    // Las piezas de cabina NO se renderizan dos veces en primera persona.
    const glm::vec3 gray{0.33f,0.42f,0.51f};
    const glm::vec3 white{0.93f,0.94f,0.91f};
    const glm::vec3 blue{0.25f,0.60f,0.83f};
    const glm::vec3 red{0.87f,0.24f,0.23f};
    // Dorsal de cabina y luneta oscura.
    add(0, {0.17f,0.29f,0}, {0,0,0}, {0.82f,0.12f,0.59f}, gray);
    add(0, {0.32f,0.40f,0}, {0,0,0}, {0.40f,0.11f,0.52f}, {0.11f,0.25f,0.35f});
    // Linea longitudinal del fuselaje (ambos laterales).
    for(float side : {-1.0f, 1.0f}) {
        add(0, {-0.35f,-0.04f,side*0.16f}, {0,0,0}, {1.19f,0.045f,0.025f}, blue);
        // Borde coloreado del ala y puntera.
        add(0, {-0.10f,0.005f,side*1.03f}, {0,0,0}, {0.40f,0.04f,0.10f}, red);
        add(0, {-0.29f,-0.01f,side*0.64f}, {0,0,0}, {0.10f,0.065f,0.82f}, white);
        // Tomas de aire laterales.
        add(0, {-0.50f,-0.05f,side*0.31f}, {0,0,0}, {0.43f,0.19f,0.095f}, gray);
        add(0, {-0.28f,-0.04f,side*0.365f}, {0,0,0}, {0.08f,0.12f,0.012f}, {0.06f,0.10f,0.15f});
        // Marca blanca sobre el estabilizador horizontal.
        add(0, {-1.08f,0.079f,side*0.43f}, {0,0,0}, {0.16f,0.012f,0.24f}, white);
    }
    // Turbina trasera oscura con reborde circular.
    add(1, {-1.37f,0.02f,0}, {0,0,-90}, {0.195f,0.09f,0.195f}, gray);
    add(1, {-1.425f,0.02f,0}, {0,0,-90}, {0.15f,0.012f,0.15f}, {0.05f,0.09f,0.12f});
    // Deriva y timon separados con marca celeste.
    add(0, {-1.21f,0.26f,0}, {0,0,-18}, {0.08f,0.35f,0.063f}, blue);
    // Puntas de nariz y costados del morro.
    add(0, {0.70f,0.0f,0}, {0,0,0}, {0.10f,0.095f,0.19f}, white);
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

void Aircraft::collect_cockpit(std::vector<RenderItem>& items) const {
    // Omitimos las primeras 8 piezas externas para que el fuselaje
    // no obstruya la vista de primera persona; dejamos tablero y marco.
    for (std::size_t i = 8; i < cockpit_end_; ++i) {
        const Part& p = parts_[i];
        items.push_back({&meshes_[p.mesh_index], pose_ * p.local, p.color});
    }
}
