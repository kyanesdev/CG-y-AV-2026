#include <cstdlib>
#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "modules/Aircraft.h"
#include "modules/Shader.h"
#include "modules/ResourceManager.h"

int main() {
    glfwSetErrorCallback([](int e,const char* s){std::cerr<<"GLFW "<<e<<": "<<s<<'\n';});
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(1000,700,"CGyAV - Guia 4 - Aeronave",nullptr,nullptr);
    if (!window) {glfwTerminate();return EXIT_FAILURE;}
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {glfwDestroyWindow(window);glfwTerminate();return EXIT_FAILURE;}
    glfwSetFramebufferSizeCallback(window,[](GLFWwindow*,int w,int h){glViewport(0,0,w,h);});
    glfwSwapInterval(1);
    int result=EXIT_SUCCESS;
    {
        try {
            ResourceManager resources("assets");
            const auto& source=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
            Shader shader;
            if (!shader.compile_from_source(source.vs,source.fs)) throw std::runtime_error("Error compilando shaders");
            Aircraft plane;
            plane.init();
            std::cout<<"Mallas distintas: "<<plane.mesh_count()<<" | Piezas: "<<plane.piece_count()<<'\n';
            std::cout<<"ESC: salir | ESPACIO: pausar/reanudar cabeceo\n";
            const int model_loc=shader.loc("uModel");
            const int color_loc=shader.loc("uColor");
            const int adjust_loc=shader.loc("uAjuste");
            glEnable(GL_DEPTH_TEST);
            bool paused=false;
            bool last_space=false;
            float pitch=0.0f;
            double last_time=glfwGetTime();
            while (!glfwWindowShouldClose(window)) {
                const double now=glfwGetTime();
                const float dt=static_cast<float>(now-last_time);
                last_time=now;
                if (glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(window,true);
                const bool space=glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS;
                if(space && !last_space) paused=!paused;
                last_space=space;
                if(!paused) pitch+=dt;
                plane.update(glm::vec3(0.0f),glm::vec3(0.0f,0.0f,glm::radians(17.0f)*std::sin(pitch)));
                std::vector<RenderItem> items;
                plane.collect(items);
                int width=0,height=0;
                glfwGetFramebufferSize(window,&width,&height);
                if(width==0 || height==0){glfwPollEvents();continue;}
                glViewport(0,0,width,height);
                // Vista fija de taller: aun NO implementamos una camara (Guia 05).
                glm::mat4 adjust(1.0f);
                adjust=glm::scale(adjust,glm::vec3(0.63f*static_cast<float>(height)/width,0.82f,-0.65f));
                adjust=glm::rotate(adjust,glm::radians(35.0f),glm::vec3(0,1,0));
                adjust=glm::rotate(adjust,glm::radians(-22.0f),glm::vec3(1,0,0));
                shader.set_uniform(adjust_loc,adjust);
                glClearColor(51.0f/256,55.0f/256,76.0f/256,1);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                shader.use();
                for(const auto& item:items) {
                    shader.set_uniform(model_loc,item.model);
                    shader.set_uniform(color_loc,item.color);
                    glBindVertexArray(item.mesh->vao());
                    glDrawElements(GL_TRIANGLES,item.mesh->count(),GL_UNSIGNED_INT,nullptr);
                }
                glBindVertexArray(0);
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        } catch (const std::exception& e) {std::cerr<<"Error: "<<e.what()<<'\n';result=EXIT_FAILURE;}
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
