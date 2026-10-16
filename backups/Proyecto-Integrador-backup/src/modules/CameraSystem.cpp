#include "CameraSystem.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

CameraSystem::CameraSystem(int width, int height) {
    set_viewport(width, height);
    update(glm::vec3(0.0f), glm::vec3(0.0f), CameraCommand{});
}

void CameraSystem::set_viewport(int width, int height) {
    // No dividir por cero al minimizar la ventana.
    if (width <= 0 || height <= 0) return;
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    data_.projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void CameraSystem::update(const glm::vec3& target,
                          const glm::vec3& aircraft_angles,
                          const CameraCommand& cmd) {
    (void)aircraft_angles; // Reservado para una eventual camara de cabina.
    constexpr float half_pi = 1.57079632679f;
    yaw_ += cmd.yaw_delta;
    pitch_ = std::clamp(pitch_ + cmd.pitch_delta, -0.9f * half_pi, 0.9f * half_pi);
    distance_ = std::clamp(distance_ + cmd.dist_delta, 2.0f, 25.0f);
    const glm::vec3 displacement(
        distance_ * std::cos(pitch_) * std::cos(yaw_),
        distance_ * std::sin(pitch_),
        distance_ * std::cos(pitch_) * std::sin(yaw_));
    data_.view = glm::lookAt(target + displacement, target, glm::vec3(0,1,0));
}
