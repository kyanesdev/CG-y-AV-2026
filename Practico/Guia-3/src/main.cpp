#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "modules/Mesh.h"
#include "modules/Shader.h"
#include "modules/Primitives.h"
#include "modules/ResourceManager.h"
namespace {
void print_data(const char* name, const MeshData& data) {
    std::cout << name << ": " << data.vertices.size() << " vertices, "
              << data.indices.size() << " indices; normales/indices "
              << (primitives::check_winding(data) ? "OK" : "ERROR") << '\n';
    if (!primitives::check_winding(data)) throw std::runtime_error("Indices invertidos");
}
void draw(const Mesh& mesh, const Shader& shader, int model_loc, int color_loc,
          const glm::mat4& model, const glm::vec3& color) {
    shader.set_uniform(model_loc, model);
    shader.set_uniform(color_loc, color);
    glBindVertexArray(mesh.vao());
    glDrawElements(GL_TRIANGLES, mesh.count(), GL_UNSIGNED_INT, nullptr);
}
}
int main() {
    glfwSetErrorCallback([](int code, const char* msg){ std::cerr << "GLFW " << code << ": " << msg << '\n'; });
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(800,600,"CGyAV - Guia 3 - Primitivas y modelo",nullptr,nullptr);
    if (!window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) { glfwDestroyWindow(window); glfwTerminate(); return EXIT_FAILURE; }
    glfwSetFramebufferSizeCallback(window,[](GLFWwindow*,int w,int h){ glViewport(0,0,w,h); });
    int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
    glViewport(0,0,width,height);
    glfwSwapInterval(1);
    int result=EXIT_SUCCESS;
    {
      try {
        ResourceManager resources("assets");
        const auto& source=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
        Shader shader;
        if (!shader.compile_from_source(source.vs,source.fs)) throw std::runtime_error("Error compilando shaders");
        const MeshData cube_data=primitives::cube();
        const MeshData cyl_data=primitives::cylinder(.5f, 1.2f, 8, 1);
        const MeshData cone_data=primitives::cone(.5f,glm::radians(35.0f),20,2);
        print_data("Cubo",cube_data); print_data("Cilindro",cyl_data); print_data("Cono",cone_data);
        Mesh cube; cube.load(cube_data);
        Mesh cyl; cyl.load(cyl_data);
        Mesh cone; cone.load(cone_data);
        const int model_loc=shader.loc("uModel");
        const int color_loc=shader.loc("uColor");
        const int adjust_loc=shader.loc("uAjuste");
        glEnable(GL_DEPTH_TEST);
        while (!glfwWindowShouldClose(window)) {
            if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(window,true);
            glfwGetFramebufferSize(window,&width,&height);
            if(width>0 && height>0) {
                // Matriz dada por la catedra, aun no es una camara.
                const glm::mat4 adjust=glm::scale(glm::mat4(1.0f),
                    glm::vec3(static_cast<float>(height)/width,1.0f,-1.0f));
                shader.set_uniform(adjust_loc,adjust);
            }
            glClearColor(51.0f/256,55.0f/256,76.0f/256,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.use();
            glm::mat4 cube_model=glm::translate(glm::mat4(1.0f),glm::vec3(-.68f,0,0));
            cube_model=glm::rotate(cube_model,glm::radians(25.0f),glm::vec3(1,1,0));
            cube_model=glm::scale(cube_model,glm::vec3(.55f));
            draw(cube,shader,model_loc,color_loc,cube_model,glm::vec3(1.0f,.35f,.35f));
            glm::mat4 cyl_model=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));
            cyl_model=glm::rotate(cyl_model,glm::radians(22.0f),glm::vec3(1,0,0));
            cyl_model=glm::scale(cyl_model,glm::vec3(.48f));
            draw(cyl,shader,model_loc,color_loc,cyl_model,glm::vec3(.3f,.8f,1.0f));
            glm::mat4 cone_model=glm::translate(glm::mat4(1.0f),glm::vec3(.68f,0,0));
            cone_model=glm::rotate(cone_model,glm::radians(-20.0f),glm::vec3(0,0,1));
            cone_model=glm::scale(cone_model,glm::vec3(.48f));
            draw(cone,shader,model_loc,color_loc,cone_model,glm::vec3(1.0f,.82f,.3f));
            glBindVertexArray(0);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
      } catch(const std::exception& e) {std::cerr<<"Error: "<<e.what()<<'\n';result=EXIT_FAILURE;}
    }
    glfwDestroyWindow(window); glfwTerminate(); return result;
}
