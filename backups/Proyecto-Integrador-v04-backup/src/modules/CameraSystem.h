#pragma once
#include <glm/glm.hpp>

struct CameraData {
    glm::mat4 view{1.0f};
    glm::mat4 projection{1.0f};
};

struct CameraCommand {
    float yaw_delta = 0.0f;
    float pitch_delta = 0.0f;
    float dist_delta = 0.0f;
};

// Tres modos, seleccionables solo con teclado (1 / 2 / 3).
enum class CameraMode { Cockpit = 1, Chase = 2, Flyby = 3 };

class CameraSystem {
public:
    CameraSystem(int width, int height);
    void set_mode(CameraMode mode, const glm::vec3& position, const glm::vec3& angles);
    CameraMode mode() const { return mode_; }
    void update(const glm::vec3& position, const glm::vec3& angles,
                const CameraCommand& cmd, float dt = 1.0f/60.0f);
    void set_viewport(int width, int height);
    const CameraData& data() const { return data_; }
private:
    CameraMode mode_ = CameraMode::Chase;
    glm::vec3 smoothed_offset_{0.0f};
    int shot_index_ = 0;
    float locked_yaw_ = 0.0f;
    float orbit_yaw_ = 0.0f;
    float orbit_pitch_ = 0.0f;
    int viewport_width_ = 1100, viewport_height_ = 720;
    float chase_distance_ = 17.0f;
    CameraData data_{};
};
