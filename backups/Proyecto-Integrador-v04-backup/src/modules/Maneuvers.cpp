#include "Maneuvers.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

namespace {
float shortest_angle(float a) {
    while(a > glm::pi<float>()) a -= glm::two_pi<float>();
    while(a < -glm::pi<float>()) a += glm::two_pi<float>();
    return a;
}
}

void Maneuvers::reset() {
    tonel_done_ = false;
    looping_done_ = false;
    roll_sum_ = roll_time_ = 0.0f;
    roll_dir_ = 0;
    looping_active_ = false;
    loop_sum_ = loop_time_ = 0.0f;
    loop_dir_ = 0;
}

void Maneuvers::update(const dlfdm::AircraftState& previous,
                       const dlfdm::AircraftState& current,
                       const glm::vec3& from, const glm::vec3& to, float dt) {
    if(dt <= 0.0f) return;
    const float speed = glm::length(current.body_velocity);
    const float altitude = to.y;

    // TONEL: alabeo continuo >= 360 grados con actitud razonablemente estable.
    if(!tonel_done_) {
        // En el integrador por cuaterniones phi vuelve a [-pi,pi].
        // La velocidad angular longitudinal p NO tiene discontinuidad.
        const float delta = 0.5f*(current.body_omega.x+previous.body_omega.x)*dt;
        const int dir = delta > 0.0f ? 1 : -1;
        const bool valid = speed > 55.0f && altitude > 35.0f &&
            std::abs(delta) > 0.001f && std::abs(delta) < 0.20f &&
            std::abs(current.theta) < 0.85f;
        if(!valid || (roll_dir_ != 0 && dir != roll_dir_)) {
            roll_sum_ = roll_time_ = 0.0f;
            roll_dir_ = 0;
        } else {
            if(roll_dir_ == 0) {
                roll_dir_ = dir;
                roll_start_yaw_ = current.psi;
                roll_start_pitch_ = current.theta;
            }
            roll_sum_ += delta;
            roll_time_ += dt;
            if(roll_time_ > 15.0f ||
               std::abs(shortest_angle(current.psi - roll_start_yaw_)) > 1.1f ||
               std::abs(current.theta - roll_start_pitch_) > 0.65f) {
                roll_sum_ = roll_time_ = 0.0f;
                roll_dir_ = 0;
            } else if(std::abs(roll_sum_) >= glm::two_pi<float>() - 0.12f) {
                tonel_done_ = true;
            }
        }
    }

    // LOOPING: giro completo de la DIRECCION DEL DESPLAZAMIENTO en un plano vertical.
    // No basta mantener elevador: el FDM limita theta a +/-1.5 rad. El algoritmo
    // NO premia falsos loopings mientras el avion permanece vertical.
    if(looping_done_ || speed <= 55.0f || altitude <= 35.0f) {
        if(!looping_done_) looping_active_ = false;
        return;
    }
    const glm::vec3 velocity = (to - from) / dt;
    const glm::vec2 horizontal{velocity.x, velocity.z};
    if(!looping_active_) {
        const float hnorm = glm::length(horizontal);
        if(std::abs(current.body_omega.y) < 0.16f || hnorm < 30.0f) return;
        horizontal_start_ = horizontal / hnorm;
        previous_flight_angle_ = std::atan2(velocity.y, hnorm);
        loop_dir_ = current.body_omega.y > 0 ? 1 : -1;
        loop_sum_ = loop_time_ = 0.0f;
        looping_active_ = true;
    }
    const float forward = glm::dot(horizontal, horizontal_start_);
    const float sideways = horizontal.x * horizontal_start_.y - horizontal.y * horizontal_start_.x;
    const float angle = std::atan2(velocity.y, forward);
    const float difference = shortest_angle(angle - previous_flight_angle_);
    previous_flight_angle_ = angle;
    loop_time_ += dt;
    if(loop_time_ > 28.0f || std::abs(sideways) > std::max(30.0f, glm::length(velocity)*0.65f) ||
       (loop_dir_ * current.body_omega.y < -0.12f) || std::abs(difference) > 0.30f) {
        looping_active_ = false;
        return;
    }
    // Acumular solo giros coherentes; oscilaciones no suman 360 por casualidad.
    if(difference * static_cast<float>(loop_dir_) < -0.025f) {
        looping_active_ = false;
        return;
    }
    loop_sum_ += difference;
    if(std::abs(loop_sum_) >= glm::two_pi<float>() - 0.15f)
        looping_done_ = true;
}
