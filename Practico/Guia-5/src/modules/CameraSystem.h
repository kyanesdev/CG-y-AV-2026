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

class CameraSystem {
public:
    CameraSystem(int width, int height);
    void update(const glm::vec3& target, const glm::vec3& aircraft_angles,
                const CameraCommand& cmd);
    void set_viewport(int width, int height);
    const CameraData& data() const { return data_; }
private:
    float yaw_ = 0.95f;
    float pitch_ = 0.30f;
    float distance_ = 6.5f;
    CameraData data_{};
};
