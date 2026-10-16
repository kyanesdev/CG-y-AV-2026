#pragma once
#include <glm/glm.hpp>
#include <dlfdm/defines.h>

// Reconoce movimientos medidos del FDM: sin imponer rotaciones al avion.
// El looping usa un giro completo de la trayectoria en su plano vertical.
class Maneuvers {
public:
    void reset();
    void update(const dlfdm::AircraftState& previous,
                const dlfdm::AircraftState& current,
                const glm::vec3& from, const glm::vec3& to, float dt);
    bool tonel_done() const { return tonel_done_; }
    bool looping_done() const { return looping_done_; }
    int score() const { return (tonel_done_ ? 250 : 0) + (looping_done_ ? 250 : 0); }
private:
    bool tonel_done_ = false;
    bool looping_done_ = false;
    float roll_sum_ = 0.0f, roll_time_ = 0.0f;
    int roll_dir_ = 0;
    float roll_start_yaw_ = 0.0f, roll_start_pitch_ = 0.0f;

    bool looping_active_ = false;
    float loop_sum_ = 0.0f, loop_time_ = 0.0f;
    float previous_flight_angle_ = 0.0f;
    glm::vec2 horizontal_start_{1.0f, 0.0f};
    int loop_dir_ = 0;
};
