#include "InputHandler.h"
#include <algorithm>

void InputHandler::update(GLFWwindow* window, float frame_dt) {
    command_ = {};
    double x=0, y=0;
    glfwGetCursorPos(window, &x, &y);
    if (first_mouse_) { previous_x_=x; previous_y_=y; first_mouse_=false; }
    const double dx=x-previous_x_, dy=y-previous_y_;
    previous_x_=x; previous_y_=y;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
        command_.yaw_delta = static_cast<float>(dx*0.005); // sentido validado en Guia 05
        command_.pitch_delta = static_cast<float>(dy*0.005);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS)
        command_.dist_delta = static_cast<float>(dy*0.02);

    // Flechas representan el baston: arriba=empujar => morro abajo, delta_e positivo.
    // Entradas continuas: rampa por segundo y deflexiones dentro de topes del modelo.
    constexpr float surface_rate = 0.20f; // rad/s
    constexpr float throttle_rate = 0.25f; // 1/s
    const auto pressed=[window](int key){return glfwGetKey(window,key)==GLFW_PRESS;};
    if (pressed(GLFW_KEY_UP)) controls_.elevator += surface_rate*frame_dt;
    if (pressed(GLFW_KEY_DOWN)) controls_.elevator -= surface_rate*frame_dt;
    if (pressed(GLFW_KEY_RIGHT)) controls_.aileron += surface_rate*frame_dt;
    if (pressed(GLFW_KEY_LEFT)) controls_.aileron -= surface_rate*frame_dt;
    // Tabla de la catedra: pedal derecho => delta_r < 0.
    if (pressed(GLFW_KEY_D)) controls_.rudder -= surface_rate*frame_dt;
    if (pressed(GLFW_KEY_A)) controls_.rudder += surface_rate*frame_dt;
    if (pressed(GLFW_KEY_W)) controls_.throttle += throttle_rate*frame_dt;
    if (pressed(GLFW_KEY_S)) controls_.throttle -= throttle_rate*frame_dt;
}
