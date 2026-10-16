#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
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
#include "modules/Course.h"
#include "modules/Hud.h"
#include "modules/InputHandler.h"
#include "modules/Maneuvers.h"
#include "modules/ResourceManager.h"
#include "modules/Scene.h"
#include "modules/Shader.h"

struct FlightData { glm::vec3 position; glm::vec3 angles; };
// La posicion del modelo fisico esta en NED con origen ~5000 m de altitud.
// Para graficos restamos el origen del trim: comienza a 120 m sobre el campus.
// No tocamos NINGUNA ecuacion ni estado interno del FDM.
static FlightData to_world(const dlfdm::AircraftState& state,
                           const dlfdm::AircraftState& origin) {
    FlightData out;
    out.position = {
        state.inertial_position.y-origin.inertial_position.y,
        120.0f - (state.inertial_position.z-origin.inertial_position.z),
        -(state.inertial_position.x-origin.inertial_position.x)
    };
    // X delante, Y arriba, Z transversal. Signo de roll validado por el alumno.
    out.angles = {state.phi, glm::half_pi<float>()-state.psi, state.theta};
    return out;
}

struct WindowContext {
    CameraSystem* camera = nullptr;
    bool paused = false;
    bool menu_open = true;
    int menu_selected = 0;
    bool wireframe = false;
    bool reset_requested = false;
    bool boundary_reached = false;
    bool school_mode = true;
    glm::vec3 plane_position{0,120,0};
    glm::vec3 plane_angles{0,0,0};
};

static void framebuffer_callback(GLFWwindow* win,int w,int h) {
    glViewport(0,0,w,h);
    auto* ctx=static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
    if(ctx && ctx->camera) ctx->camera->set_viewport(w,h);
}
static void key_callback(GLFWwindow* win,int key,int,int action,int) {
    if(action!=GLFW_PRESS) return;
    auto* ctx=static_cast<WindowContext*>(glfwGetWindowUserPointer(win));
    if(!ctx) return;
    if(ctx->menu_open) {
        if(key==GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(win,GLFW_TRUE); return; }
        if(key==GLFW_KEY_UP) ctx->menu_selected=(ctx->menu_selected+2)%3;
        if(key==GLFW_KEY_DOWN) ctx->menu_selected=(ctx->menu_selected+1)%3;
        if(key==GLFW_KEY_ENTER || key==GLFW_KEY_KP_ENTER) {
            if(ctx->menu_selected==2) glfwSetWindowShouldClose(win,GLFW_TRUE);
            else {
                if(ctx->menu_selected==1) ctx->reset_requested=true;
                ctx->menu_open=false;
                if(!ctx->boundary_reached) ctx->paused=false;
            }
        }
        return;
    }
    if(key==GLFW_KEY_ESCAPE) {
        ctx->menu_open=true;
        ctx->menu_selected=0;
        ctx->paused=true;
        return;
    }
    if(key==GLFW_KEY_1 || key==GLFW_KEY_2 || key==GLFW_KEY_3) {
        ctx->camera->set_mode(static_cast<CameraMode>(key-GLFW_KEY_1+1),
                              ctx->plane_position,ctx->plane_angles);
    }
    if(key==GLFW_KEY_R) ctx->reset_requested=true;
    if(key==GLFW_KEY_T) ctx->school_mode=!ctx->school_mode;
    if((key==GLFW_KEY_SPACE || key==GLFW_KEY_P) && !ctx->boundary_reached)
        ctx->paused=!ctx->paused;
    if(key==GLFW_KEY_F) {
        ctx->wireframe=!ctx->wireframe;
        glPolygonMode(GL_FRONT_AND_BACK,ctx->wireframe?GL_LINE:GL_FILL);
    }
}

int main() {
    glfwSetErrorCallback([](int n,const char* s){std::cerr<<"GLFW "<<n<<": "<<s<<'\n';});
    if(!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(1100,720,"AeroIUA: Mateando en las nubes",nullptr,nullptr);
    if(!window){glfwTerminate();return EXIT_FAILURE;}
    glfwMakeContextCurrent(window);
    if(!gladLoadGL(glfwGetProcAddress)){glfwDestroyWindow(window);glfwTerminate();return EXIT_FAILURE;}
    int w=0,h=0;glfwGetFramebufferSize(window,&w,&h);glViewport(0,0,w,h);
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
            const auto& source=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
            Shader shader;
            if(!shader.compile_from_source(source.vs,source.fs)) throw std::runtime_error("Fallo shader");
            const int model_loc=shader.loc("uModel"),color_loc=shader.loc("uColor");
            const int view_loc=shader.loc("uView"),projection_loc=shader.loc("uProjection");
            Aircraft aircraft; aircraft.init();
            Scene scene; scene.init();
            Course course; course.init();
            Maneuvers maneuvers;
            Hud hud; hud.init();

            const auto params=dlfdm::jettrainer::load_model();
            dlfdm::FDMSolver fdm(params);
            const auto trim=dlfdm::jettrainer::get_trim_condition(dlfdm::jettrainer::TrimCondition::kISA5000TAS150);
            fdm.setState(trim.state);
            InputHandler input; input.reset_controls(trim.controls);
            FlightData flight=to_world(fdm.getState(),trim.state);
            camera.set_mode(CameraMode::Chase,flight.position,flight.angles);
            camera.update(flight.position,flight.angles,CameraCommand{});

            glEnable(GL_DEPTH_TEST);
            std::cout << "AeroIUA v0.3 | Menu: flechas + ENTER | ESC vuelve al menu\n"
                      << "1 cabina | 2 seguimiento | 3 toma estatica (se recoloca al alejarse)\n"
                      << "Flechas controlan avion | A/D timon | W/S potencia | P pausa | R reinicio\n"
                      << "Aros: 100 pts c/u | Tonel: 250 | Looping real: 250 | SHIFT maniobras | T tiempo\n";
            constexpr double fixed_dt = 1.0/120.0;
            double previous_clock=glfwGetTime(),accumulator=0.0;
            double next_ui=0.0;
            int old_menu_selection=-1, old_menu_width=0, old_menu_height=0;
            std::vector<RenderItem> items;
            items.reserve(400);
            std::vector<RenderItem> overlay;
            overlay.reserve(8);

            while(!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                const double now=glfwGetTime();
                const double frame_dt=std::clamp(now-previous_clock,0.0,0.25);
                previous_clock=now;

                if(ctx.reset_requested) {
                    fdm.setState(trim.state);
                    input.reset_controls(trim.controls);
                    input.reset_mouse();
                    course.reset();
                    maneuvers.reset();
                    accumulator=0;
                    ctx.paused=false;
                    ctx.reset_requested=false;
                    ctx.boundary_reached=false;
                    flight=to_world(fdm.getState(),trim.state);
                    camera.set_mode(CameraMode::Chase,flight.position,flight.angles);
                    std::cout << "Circuito reiniciado.\n";
                }

                // Mientras hay menu o pausa, no se alteran los controles del FDM.
                if(!ctx.menu_open && !ctx.paused)
                    input.update(window,static_cast<float>(frame_dt));
                auto controls=input.controls();
                controls.elevator=std::clamp(controls.elevator,params.min_elevator,params.max_elevator);
                controls.aileron=std::clamp(controls.aileron,params.min_aileron,params.max_aileron);
                controls.rudder=std::clamp(controls.rudder,-params.max_rudder,params.max_rudder);
                controls.throttle=std::clamp(controls.throttle,0.0f,1.0f);
                input.set_controls(controls);

                if(ctx.paused || ctx.menu_open) accumulator=0.0;
                else accumulator+=frame_dt*(ctx.school_mode?0.60:1.0);
                while(accumulator>=fixed_dt) {
                    const auto previous_state=fdm.getState();
                    const glm::vec3 from=to_world(previous_state,trim.state).position;
                    fdm.update(input.controls());
                    const auto& current_state=fdm.getState();
                    const glm::vec3 to=to_world(current_state,trim.state).position;
                    const bool old_tonel=maneuvers.tonel_done();
                    const bool old_loop=maneuvers.looping_done();
                    maneuvers.update(previous_state,current_state,from,to,static_cast<float>(fixed_dt));
                    if(!old_tonel && maneuvers.tonel_done())
                        std::cout << "TONEL comprobado +250 puntos\n";
                    if(!old_loop && maneuvers.looping_done())
                        std::cout << "LOOPING comprobado +250 puntos\n";
                    if(course.test_crossing(from,to)) {
                        std::cout << "Aro " << course.completed() << "/5 superado; +100 puntos. Total "
                                  << course.score()+maneuvers.score()
                                  << (course.finished()?" | 5 aros completos!":"") << '\n';
                    }
                    accumulator-=fixed_dt;
                }
                const auto& state=fdm.getState();
                flight=to_world(state,trim.state);
                // Delimitamos el area disponible sin tocar las fuerzas del FDM.
                if(!ctx.boundary_reached && (std::abs(flight.position.x)>4900.0f ||
                                             std::abs(flight.position.z)>4900.0f)) {
                    ctx.boundary_reached=true;
                    ctx.paused=true;
                    std::cout << "Fin del area de vuelo. Pulse R para reiniciar.\n";
                }
                ctx.plane_position=flight.position;
                ctx.plane_angles=flight.angles;
                aircraft.update(flight.position,flight.angles);
                camera.update(flight.position,flight.angles,
                              (!ctx.paused && !ctx.menu_open)?input.camera_cmd():CameraCommand{},
                              static_cast<float>(frame_dt));

                if(now>=next_ui) {
                    const float speed=glm::length(state.body_velocity)*3.6f;
                    const float relative_alt=flight.position.y-
                        Scene::terrain_height(flight.position.x,flight.position.z);
                    hud.update_text(speed,relative_alt,glm::degrees(state.phi),
                                    course.completed(),course.score()+maneuvers.score(),
                                    ctx.paused || ctx.menu_open,
                                    maneuvers.looping_done(),maneuvers.tonel_done(),
                                    controls.throttle*100.0f,ctx.school_mode);
                    const char* cam=camera.mode()==CameraMode::Cockpit?"CABINA":
                        camera.mode()==CameraMode::Chase?"SEGUIMIENTO":"CINEMATICA";
                    std::ostringstream title;
                    title << "AeroIUA | " << (ctx.menu_open ? "MENU" : cam)
                          << " | Aro " << course.completed() << "/5 | "
                          << course.score()+maneuvers.score() << " pts";
                    if(course.finished() && maneuvers.looping_done() && maneuvers.tonel_done())
                        title << " | ENTRENAMIENTO COMPLETO";
                    if(ctx.boundary_reached) title << " | LIMITE - PULSE R";
                    else if(ctx.paused) title << " | PAUSA";
                    glfwSetWindowTitle(window,title.str().c_str());
                    next_ui=now+0.20;
                }
                glfwGetFramebufferSize(window,&w,&h);
                if(w<=0 || h<=0) continue;
                items.clear();
                scene.collect(items);
                course.collect(items);
                // Cabina: solo tablero y marcos; otros modos: avion completo.
                if(camera.mode()==CameraMode::Cockpit) aircraft.collect_cockpit(items);
                else aircraft.collect(items);

                glClearColor(0.47f,0.69f,0.88f,1.0f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                shader.use();
                shader.set_uniform(view_loc,camera.data().view);
                shader.set_uniform(projection_loc,camera.data().projection);
                for(const auto& item:items) {
                    shader.set_uniform(model_loc,item.model);
                    shader.set_uniform(color_loc,item.color);
                    glBindVertexArray(item.mesh->vao());
                    glDrawElements(GL_TRIANGLES,item.mesh->count(),GL_UNSIGNED_INT,nullptr);
                }

                // HUD 2D: 3 instrumentos (velocidad, altitud AGL, roll/horizonte)
                // y puntos. Usamos el shader y la malla propia, SIN toolkit UI.
                overlay.clear();
                if(ctx.menu_open) {
                    if(old_menu_width!=w || old_menu_height!=h ||
                       old_menu_selection!=ctx.menu_selected) {
                        hud.update_menu(w,h,ctx.menu_selected);
                        old_menu_width=w; old_menu_height=h;
                        old_menu_selection=ctx.menu_selected;
                    }
                    hud.collect_menu(overlay,w,h,ctx.menu_selected);
                } else {
                    hud.collect(overlay,w,state.theta,state.phi);
                }
                shader.set_uniform(view_loc,glm::mat4(1.0f));
                shader.set_uniform(projection_loc,glm::ortho(0.f,static_cast<float>(w),
                                                             static_cast<float>(h),0.f,-1.f,1.f));
                glDisable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
                for(const auto& item:overlay) {
                    shader.set_uniform(model_loc,item.model);
                    shader.set_uniform(color_loc,item.color);
                    glBindVertexArray(item.mesh->vao());
                    glDrawElements(GL_TRIANGLES,item.mesh->count(),GL_UNSIGNED_INT,nullptr);
                }
                if(ctx.wireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glEnable(GL_DEPTH_TEST);
                glBindVertexArray(0);
                glfwSwapBuffers(window);
            }
        } catch(const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
            result=EXIT_FAILURE;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
