#pragma once
#include <GLFW/glfw3.h>
#include <dlfdm/defines.h>
#include "CameraSystem.h"

class InputHandler {
public:
    void update(GLFWwindow* window, float frame_dt);
    const dlfdm::ControlInputs& controls() const { return controls_; }
    void set_controls(const dlfdm::ControlInputs& controls) { controls_ = controls; }
    // Llamar al inicio y en reinicios para guardar el elevador de trim.
    void reset_controls(const dlfdm::ControlInputs& trim) {
        trim_elevator_ = trim.elevator;
        controls_ = trim;
    }
    CameraCommand camera_cmd() const { return command_; }
    void reset_mouse() { first_mouse_ = true; }
private:
    dlfdm::ControlInputs controls_{};
    float trim_elevator_ = 0.0f;
    CameraCommand command_{};
    double previous_x_ = 0.0, previous_y_ = 0.0;
    bool first_mouse_ = true;
};
