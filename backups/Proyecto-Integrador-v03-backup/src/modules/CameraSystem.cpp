#include "CameraSystem.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace {
// Exactamente el mismo orden de rotaciones de Aircraft::update().
glm::mat4 attitude(const glm::vec3& angles) {
    glm::mat4 r(1.0f);
    r = glm::rotate(r, angles.y, glm::vec3(0,1,0));
    r = glm::rotate(r, angles.z, glm::vec3(0,0,1));
    return glm::rotate(r, angles.x, glm::vec3(1,0,0));
}
glm::vec3 rotated(const glm::mat4& m, const glm::vec3& v) {
    return glm::vec3(m * glm::vec4(v, 0.0f));
}
}

CameraSystem::CameraSystem(int width, int height) {
    set_viewport(width, height);
    update({0,120,0}, {0,0,0}, CameraCommand{});
}

void CameraSystem::set_viewport(int width, int height) {
    if (width <= 0 || height <= 0) return;
    viewport_width_ = width;
    viewport_height_ = height;
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    // Plano lejano amplio para el escenario, sin modificar el FDM.
    data_.projection = glm::perspective(glm::radians(mode_ == CameraMode::Flyby ? 34.0f : 66.0f), aspect, 0.08f, 12000.0f);
}

void CameraSystem::set_mode(CameraMode mode, const glm::vec3& position,
                            const glm::vec3& angles) {
    mode_ = mode;
    if (mode_ == CameraMode::Flyby) {
        // Snapshot: no se desplaza al avion. Volver a pulsar 3 recoloca la camara.
        const glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), angles.y, glm::vec3(0,1,0));
        flyby_eye_ = position + rotated(yaw, {37.0f, 12.0f, 25.0f});
    }
    set_viewport(viewport_width_, viewport_height_);
}

void CameraSystem::update(const glm::vec3& position, const glm::vec3& angles,
                          const CameraCommand& cmd) {
    const glm::mat4 full = attitude(angles);
    if (mode_ == CameraMode::Cockpit) {
        // El avion mira hacia +X local y su ala esta sobre Z local.
        const glm::vec3 eye = position + rotated(full, {0.10f,0.42f,0.0f});
        const glm::vec3 forward = rotated(full, {1.0f,0.0f,0.0f});
        const glm::vec3 up = rotated(full, {0.0f,1.0f,0.0f});
        data_.view = glm::lookAt(eye, eye + forward, up);
    } else if (mode_ == CameraMode::Chase) {
        chase_distance_ = std::clamp(chase_distance_ + cmd.dist_delta, 7.0f, 35.0f);
        chase_height_ = std::clamp(chase_height_ + cmd.pitch_delta * 6.0f, 2.0f, 14.0f);
        // Sigue rumbo del avion, pero el horizonte no gira con el alabeo.
        const glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), angles.y + cmd.yaw_delta, glm::vec3(0,1,0));
        const glm::vec3 eye = position + rotated(yaw, {-chase_distance_,chase_height_,0.0f});
        data_.view = glm::lookAt(eye, position + glm::vec3(0,0.8f,0), {0,1,0});
    } else {
        // Eye es estacionario, pero observa al avion que lo sobrevuela.
        // La toma sigue siendo FIJA; al quedar muy lejos, cortamos a una nueva
        // estacion fija cercana, como una sucesion de tomas de seguimiento GTA.
        if(glm::distance(position,flyby_eye_) > 115.0f)
            set_mode(CameraMode::Flyby,position,angles);
        const glm::vec3 direction = position - flyby_eye_;
        if (glm::dot(direction,direction) > 0.01f)
            data_.view = glm::lookAt(flyby_eye_, position, {0,1,0});
    }
}
