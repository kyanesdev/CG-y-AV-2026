#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <dlfdm/fdmsolver.h>
#include <dlfdm/models/aircraft/jettrainer.h>
#include "modules/Aircraft.h"
#include "modules/CameraSystem.h"
#include "modules/InputHandler.h"
#include "modules/Shader.h"
#include "modules/ResourceManager.h"
#include "modules/Terrain.h"

struct WindowContext { CameraSystem* camera; bool paused=false, wireframe=false, atLimit=false; };
static void framebuffer_callback(GLFWwindow* win, int w, int h) {
    glViewport(0,0,w,h);
    auto* ctx=static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
    if(ctx && ctx->camera) ctx->camera->set_viewport(w,h);
}
static void key_callback(GLFWwindow* win, int key, int, int action, int) {
    if(action!=GLFW_PRESS) return;
    auto* ctx=static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
    if(key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win,GLFW_TRUE);
    if(!ctx) return;
    if(key==GLFW_KEY_SPACE) {
        if(ctx->atLimit) std::cout<<"Limite del mundo: cerrar y reiniciar para continuar.\n";
        else {ctx->paused=!ctx->paused;std::cout<<(ctx->paused?"Pausa\n":"Reanudar\n");}
    }
    if(key==GLFW_KEY_F) {ctx->wireframe=!ctx->wireframe; glPolygonMode(GL_FRONT_AND_BACK,ctx->wireframe?GL_LINE:GL_FILL);}
}
struct FlightData { glm::vec3 position; glm::vec3 angles; };
static FlightData to_world(const dlfdm::AircraftState& ned) {
    FlightData out;
    out.position={ned.inertial_position.y,-ned.inertial_position.z,-ned.inertial_position.x};
    out.angles={-ned.phi,ned.theta,glm::half_pi<float>()-ned.psi};
    return out;
}

int main() {
    glfwSetErrorCallback([](int n,const char* s){std::cerr<<"GLFW "<<n<<": "<<s<<'\n';});
    if(!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(1120,760,"CGyAV - Guia 07 - Terreno",nullptr,nullptr);
    if(!window){glfwTerminate();return EXIT_FAILURE;}
    glfwMakeContextCurrent(window);
    if(!gladLoadGL(glfwGetProcAddress)){glfwDestroyWindow(window);glfwTerminate();return EXIT_FAILURE;}
    int w=0,h=0; glfwGetFramebufferSize(window,&w,&h); glViewport(0,0,w,h);
    CameraSystem camera(w,h);
    WindowContext ctx{&camera};
    glfwSetWindowUserPointer(window,&ctx);
    glfwSetFramebufferSizeCallback(window,framebuffer_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSwapInterval(1);
    int result=EXIT_SUCCESS;
    {
        try {
            ResourceManager resources("assets");
            const auto& src=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
            Shader shader;
            if(!shader.compile_from_source(src.vs,src.fs)) throw std::runtime_error("Fallo shader");
            Aircraft aircraft; aircraft.init();
            const TerrainConfig config;
            float minY=0, maxY=0;
            const MeshData terrainData = make_terrain(config,minY,maxY);
            Mesh terrain; terrain.load(terrainData);
            std::cout << "Terreno: " << config.cells << "x" << config.cells << " celdas, "
                      << terrainData.vertices.size() << " vertices, "
                      << terrainData.indices.size()/3 << " triangulos, "
                      << config.cells*config.spacing << " m por lado. Alturas ["
                      << minY << ", " << maxY << "] m\n";
            dlfdm::FDMSolver fdm(dlfdm::jettrainer::load_model());
            const auto trim=dlfdm::jettrainer::get_trim_condition(dlfdm::jettrainer::TrimCondition::kISA5000TAS150);
            fdm.setState(trim.state);
            const glm::vec3 origin=to_world(trim.state).position; // referenciamos el FDM localmente
            InputHandler input; input.set_controls(trim.controls);
            const auto params=dlfdm::jettrainer::load_model();
            const int model_loc=shader.loc("uModel"),color_loc=shader.loc("uColor");
            const int view_loc=shader.loc("uView"),projection_loc=shader.loc("uProjection");
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            std::cout << "SPACE pausa | F alambre | ESC salir | Mouse izquierdo orbita, derecho zoom\n";
            std::cout << "Flechas avion | A,D timon | W,S potencia | Limite del mundo: pausa\n";
            constexpr double step=1.0/120.0;
            double before=glfwGetTime(), accumulator=0.0, last_log=before;
            bool limit_warned=false;
            while(!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                const double now=glfwGetTime();
                const double frame_dt=std::clamp(now-before,0.0,0.25); before=now;
                input.update(window,static_cast<float>(frame_dt));
                auto controls=input.controls();
                controls.elevator=std::clamp(controls.elevator,params.min_elevator,params.max_elevator);
                controls.aileron=std::clamp(controls.aileron,params.min_aileron,params.max_aileron);
                controls.rudder=std::clamp(controls.rudder,-params.max_rudder,params.max_rudder);
                controls.throttle=std::clamp(controls.throttle,0.0f,1.0f);
                input.set_controls(controls);
                if(!ctx.paused) accumulator+=frame_dt;
                while(accumulator>=step) {fdm.update(input.controls()); accumulator-=step;}
                const auto& state=fdm.getState();
                FlightData flight=to_world(state);
                // El FDM inicia a 5000 m MSL; el escenario representa una region local.
                // Mantener desplazamientos y actitudes, con altura visual inicial de 100 m.
                flight.position -= origin;
                flight.position.y += 100.0f;
                const float bound=config.cells*config.spacing*0.5f - 200.0f;
                if ((std::abs(flight.position.x)>bound || std::abs(flight.position.z)>bound) && !limit_warned) {
                    ctx.paused=true; ctx.atLimit=true; accumulator=0.0; limit_warned=true;
                    std::cout<<"Borde de terreno alcanzado: simulacion pausada. Reiniciar para volver al centro.\n";
                }
                aircraft.update(flight.position,flight.angles);
                camera.update(flight.position-glm::vec3(0,18,0),flight.angles,input.camera_cmd());
                if(now-last_log>=2.0){
                    std::cout<<"t="<<fdm.get_sim_time()<<"s alt. local="<<flight.position.y
                             <<"m TAS aprox="<<glm::length(state.body_velocity)<<"m/s\n";
                    last_log=now;
                }
                glfwGetFramebufferSize(window,&w,&h);
                if(w<=0||h<=0) continue;
                std::vector<RenderItem> items; aircraft.collect(items);
                shader.set_uniform(view_loc,camera.data().view);
                shader.set_uniform(projection_loc,camera.data().projection);
                glClearColor(0.60f,0.76f,0.93f,1.0f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                shader.use();
                // Terreno primero, centrado en el origen; color plano temporal.
                shader.set_uniform(model_loc,glm::mat4(1.0f));
                shader.set_uniform(color_loc,glm::vec3(0.31f,0.55f,0.22f));
                glBindVertexArray(terrain.vao());
                glDrawElements(GL_TRIANGLES,terrain.count(),GL_UNSIGNED_INT,nullptr);
                // El avion original mide ~2 unidades; se escala a ~8 m.
                for(const auto& item:items){
                    shader.set_uniform(model_loc,glm::scale(item.model,glm::vec3(4.0f)));
                    shader.set_uniform(color_loc,item.color);
                    glBindVertexArray(item.mesh->vao());
                    glDrawElements(GL_TRIANGLES,item.mesh->count(),GL_UNSIGNED_INT,nullptr);
                }
                glBindVertexArray(0);
                glfwSwapBuffers(window);
            }
        }catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';result=EXIT_FAILURE;}
    }
    glfwDestroyWindow(window); glfwTerminate(); return result;
}
