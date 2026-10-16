#include <cstdlib>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "modules/Mesh.h"
#include "modules/Shader.h"
#include "modules/Primitives.h"
#include "modules/ResourceManager.h"
int main(){
 glfwSetErrorCallback([](int code,const char* msg){std::cerr<<"GLFW "<<code<<": "<<msg<<'\n';});
 if(!glfwInit())return EXIT_FAILURE;
 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
 glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
 GLFWwindow* window=glfwCreateWindow(800,600,"CGyAV - Guia 2 - Cubo indexado",nullptr,nullptr);
 if(!window){glfwTerminate();return EXIT_FAILURE;}
 glfwMakeContextCurrent(window);
 if(!gladLoadGL(glfwGetProcAddress)){glfwDestroyWindow(window);glfwTerminate();return EXIT_FAILURE;}
 glfwSetFramebufferSizeCallback(window,[](GLFWwindow*,int w,int h){glViewport(0,0,w,h);});
 int w=0,h=0;glfwGetFramebufferSize(window,&w,&h);glViewport(0,0,w,h);
 glfwSwapInterval(1);
 int exit_code=EXIT_SUCCESS;
 {
  // Objetos Mesh/Shader destruidos antes de cerrar el contexto OpenGL.
  try{
   ResourceManager resources("assets");
   const auto& source=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
   Shader shader;
   if(!shader.compile_from_source(source.vs,source.fs)) throw std::runtime_error("No se pudo compilar/linkear el shader");
   MeshData data=primitives::cube();
   std::cout<<"Vertices: "<<data.vertices.size()<<" - Indices: "<<data.indices.size()<<'\n';
   Mesh mesh;mesh.load(data);
   glEnable(GL_DEPTH_TEST);
   while(!glfwWindowShouldClose(window)){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)glfwSetWindowShouldClose(window,true);
    glClearColor(51.0f/256.0f,55.0f/256.0f,76.0f/256.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    shader.use();glBindVertexArray(mesh.vao());
    glDrawElements(GL_TRIANGLES,mesh.count(),GL_UNSIGNED_INT,nullptr);
    glBindVertexArray(0);
    glfwSwapBuffers(window);glfwPollEvents();
   }
  }catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';exit_code=EXIT_FAILURE;}
 }
 glfwDestroyWindow(window);glfwTerminate();return exit_code;
}
