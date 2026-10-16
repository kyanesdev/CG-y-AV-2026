#include <cstdlib>
#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "modules/Aircraft.h"
#include "modules/CameraSystem.h"
#include "modules/Shader.h"
#include "modules/ResourceManager.h"

struct AppInput {
    CameraSystem* camera = nullptr;
    bool first_mouse = true;
    double previous_x = 0.0;
    double previous_y = 0.0;
};

static void framebuffer_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* input = static_cast<AppInput*>(glfwGetWindowUserPointer(window));
    if (input && input->camera) input->camera->set_viewport(width, height);
}

int main() {
    glfwSetErrorCallback([](int code, const char* msg) {
        std::cerr << "GLFW " << code << ": " << msg << '\n';
    });
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1000, 700,
        "CGyAV - Guia 5 - Camara orbital", nullptr, nullptr);
    if (!window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window); glfwTerminate(); return EXIT_FAILURE;
    }
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    CameraSystem camera(width, height);
    AppInput input{&camera};
    glfwSetWindowUserPointer(window, &input);
    glfwSetFramebufferSizeCallback(window, framebuffer_callback);
    glfwSwapInterval(1);
    int result = EXIT_SUCCESS;
    {
        try {
            ResourceManager resources("assets");
            const auto& sources = resources.load_shader_source(
                "solid", "shaders/solid.vs", "shaders/solid.fs");
            Shader shader;
            if (!shader.compile_from_source(sources.vs, sources.fs))
                throw std::runtime_error("Error compilando shaders");
            Aircraft plane;
            plane.init();
            const int model_loc = shader.loc("uModel");
            const int color_loc = shader.loc("uColor");
            const int view_loc = shader.loc("uView");
            const int projection_loc = shader.loc("uProjection");
            glEnable(GL_DEPTH_TEST);
            std::cout << "Mallas: " << plane.mesh_count()
                      << " | Piezas: " << plane.piece_count() << '\n';
            std::cout << "Arrastrar mouse izquierdo: orbitar | Derecho vertical: zoom | ESC: salir\n";
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                double x = 0.0, y = 0.0;
                glfwGetCursorPos(window, &x, &y);
                CameraCommand cmd{};
                if (input.first_mouse) {
                    input.previous_x = x;
                    input.previous_y = y;
                    input.first_mouse = false;
                }
                const double dx = x - input.previous_x;
                const double dy = y - input.previous_y;
                input.previous_x = x;
                input.previous_y = y;
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    cmd.yaw_delta = static_cast<float>(dx * 0.005);
                    cmd.pitch_delta = static_cast<float>(dy * 0.005);
                }
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
                    cmd.dist_delta = static_cast<float>(dy * 0.02);
                const glm::vec3 position(0.0f);
                const glm::vec3 angles(0.0f);
                plane.update(position, angles);
                camera.update(position, angles, cmd);
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) continue;
                std::vector<RenderItem> items;
                plane.collect(items);
                shader.set_uniform(view_loc, camera.data().view);
                shader.set_uniform(projection_loc, camera.data().projection);
                glClearColor(51.0f/256, 55.0f/256, 76.0f/256, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                shader.use();
                for (const auto& item : items) {
                    shader.set_uniform(model_loc, item.model);
                    shader.set_uniform(color_loc, item.color);
                    glBindVertexArray(item.mesh->vao());
                    glDrawElements(GL_TRIANGLES, item.mesh->count(), GL_UNSIGNED_INT, nullptr);
                }
                glBindVertexArray(0);
                glfwSwapBuffers(window);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
            result = EXIT_FAILURE;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
