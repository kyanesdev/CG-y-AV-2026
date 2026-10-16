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
        command_.dist_delta = static_cast<float>(dy*0.075);

    // Mando centrado: al soltar una flecha la superficie vuelve al trim/neutro.
    // Ya no acumulamos deflexion hasta el limite fisico del modelo.
    const auto pressed=[window](int key){return glfwGetKey(window,key)==GLFW_PRESS;};
    const float slow_aileron=0.035f; // 2 grados, vuelo escuela
    const float slow_elevator=0.045f;
    const bool aerobatic=pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    const float max_aileron=aerobatic ? 0.12f : slow_aileron;
    const float max_elevator=aerobatic ? 0.15f : slow_elevator;
    auto axis=[&](int plus,int minus) -> float {
        return (pressed(plus)?1.0f:0.0f)-(pressed(minus)?1.0f:0.0f);
    };
    auto approach=[frame_dt](float current,float target,float rate) {
        return current + std::clamp(target-current, -rate*frame_dt, rate*frame_dt);
    };
    controls_.aileron=approach(controls_.aileron,
        axis(GLFW_KEY_RIGHT,GLFW_KEY_LEFT)*max_aileron,0.25f);
    controls_.elevator=approach(controls_.elevator,
        trim_elevator_+axis(GLFW_KEY_UP,GLFW_KEY_DOWN)*max_elevator,0.17f);
    controls_.rudder=approach(controls_.rudder,
        -axis(GLFW_KEY_D,GLFW_KEY_A)*(aerobatic?0.10f:0.035f),0.20f);

    // La palanca de potencia permanece donde el piloto la deja. En un reactor
    // la aceleracion del aire NO es instantanea: el FDM determina su evolucion.
    constexpr float throttle_rate=0.40f; // 40% de mando por segundo
    controls_.throttle=std::clamp(controls_.throttle+
        axis(GLFW_KEY_W,GLFW_KEY_S)*throttle_rate*frame_dt,0.0f,1.0f);
}
