#include <algorithm>
#include <array>
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
#include <glm/gtc/matrix_inverse.hpp>
#include <dlfdm/fdmsolver.h>
#include <dlfdm/models/aircraft/jettrainer.h>
#include "modules/Aircraft.h"
#include "modules/CameraSystem.h"
#include "modules/InputHandler.h"
#include "modules/Shader.h"
#include "modules/ResourceManager.h"
#include "modules/Terrain.h"
#include "modules/Texture.h"
#include "modules/Primitives.h"

struct WindowContext {
    CameraSystem* camera;
    bool paused=false, wireframe=false, showUVTest=false, atLimit=false;
    bool gamma=true, fog=true, lighting=true, showNormals=false, flatNormals=false, lowSun=false;
};
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
    if(key==GLFW_KEY_T) ctx->showUVTest=!ctx->showUVTest;
    if(key==GLFW_KEY_G) ctx->gamma=!ctx->gamma;
    if(key==GLFW_KEY_H) ctx->fog=!ctx->fog;
    if(key==GLFW_KEY_L) ctx->lighting=!ctx->lighting;
    if(key==GLFW_KEY_N) ctx->showNormals=!ctx->showNormals;
    if(key==GLFW_KEY_B) ctx->flatNormals=!ctx->flatNormals;
    if(key==GLFW_KEY_K) ctx->lowSun=!ctx->lowSun;
}
struct FlightData { glm::vec3 position; glm::vec3 angles; };
static FlightData to_world(const dlfdm::AircraftState& ned) {
    FlightData out;
    out.position={ned.inertial_position.y,-ned.inertial_position.z,-ned.inertial_position.x};
    out.angles={-ned.phi,ned.theta,glm::half_pi<float>()-ned.psi};
    return out;
}

// El color de niebla se calcula a partir de la fila central del cubemap.
static glm::vec3 horizon_color(const std::array<const ImageData*,6>& faces) {
    glm::vec3 sum(0.0f);
    std::size_t count=0;
    for(unsigned side : {0U,1U,4U,5U}) {
        const ImageData& img=*faces[side];
        const int row=img.height/2;
        for(int x=0; x<img.width; ++x) {
            const std::size_t k=4*(std::size_t(row)*img.width+x);
            sum+=glm::vec3(img.pixels[k],img.pixels[k+1],img.pixels[k+2])/255.0f;
            ++count;
        }
    }
    return sum/static_cast<float>(count);
}

// Los mismos parametros de escena llegan a TODOS los programas.
static void send_lighting(const Shader& shader, const WindowContext& ctx,
                          const glm::vec3& eye, const glm::vec3& fogSRGB) {
    auto location=[&shader](const char* name){return glGetUniformLocation(shader.id(),name);};
    const glm::vec3 sun=glm::normalize(ctx.lowSun ? glm::vec3(-0.9f,0.18f,0.4f)
                                              : glm::vec3(-0.6f,0.80f,0.3f));
    shader.set_uniform(location("uSolDir"),sun);
    shader.set_uniform(location("uSolColor"),glm::vec3(0.70f));
    shader.set_uniform(location("uAmbiente"),glm::vec3(0.30f));
    shader.set_uniform(location("uOjo"),eye);
    shader.set_uniform(location("uNieblaColor"),fogSRGB);
    shader.set_uniform(location("uVisibilidad"),2000.0f);
    shader.set_uniform(location("uFogEnabled"),ctx.fog ? 1 : 0);
    shader.set_uniform(location("uGammaEnabled"),ctx.gamma ? 1 : 0);
    shader.set_uniform(location("uLightingEnabled"),ctx.lighting ? 1 : 0);
}

int main() {
    glfwSetErrorCallback([](int n,const char* s){std::cerr<<"GLFW "<<n<<": "<<s<<'\n';});
    if(!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(1120,760,"CGyAV - Guia 09 - Iluminacion, niebla y gamma",nullptr,nullptr);
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
            const std::string& shared=resources.load_text("lighting","shaders/iluminacion.glsl");
            const auto& src=resources.load_shader_source("solid","shaders/solid.vs","shaders/solid.fs");
            Shader shader;
            if(!shader.compile_from_source(src.vs,src.fs,shared)) throw std::runtime_error("Fallo shader");
            const auto& terrainSource=resources.load_shader_source("terrain","shaders/terreno.vs","shaders/terreno.fs");
            const auto& skySource=resources.load_shader_source("sky","shaders/background.vs","shaders/background-suelo.fs");
            Shader terrainShader, skyShader;
            if (!terrainShader.compile_from_source(terrainSource.vs,terrainSource.fs,shared) ||
                !skyShader.compile_from_source(skySource.vs,skySource.fs,shared))
                throw std::runtime_error("Fallo compilacion shader terreno/cielo");

            // 2D invertida, caras del cubemap SIN invertir. Orden: +X -X +Y -Y +Z -Z.
            const auto& grass=resources.load_image("grass","textures/grass.png",true);
            const auto& testImage=resources.load_image("uv-test","textures/uv-test.png",true);
            const std::array<std::string,6> faceNames={"right","left","top","bottom","front","back"};
            std::array<const ImageData*,6> faceData{};
            for (std::size_t i=0;i<faceNames.size();++i)
                faceData[i]=&resources.load_image("sky-"+faceNames[i],"cubemap/"+faceNames[i]+".png",false);
            const glm::vec3 fogSRGB=horizon_color(faceData);
            std::cout<<"Color medio del horizonte (sRGB): "
                     <<fogSRGB.r<<", "<<fogSRGB.g<<", "<<fogSRGB.b<<"\n";
            Texture groundTexture, uvTestTexture, skyTexture;
            groundTexture.load_2d(grass);
            uvTestTexture.load_2d(testImage);
            skyTexture.load_cubemap(faceData);
            Mesh skyCube; skyCube.load(primitives::cube(2,2,2));
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
            const int model_loc=shader.loc("uModel");
            const int matKd=shader.loc("uMaterial.kd"), matKs=shader.loc("uMaterial.ks");
            const int matNs=shader.loc("uMaterial.ns");
            const int normalMat=shader.loc("uNormalMatrix"), solidDebug=shader.loc("uShowNormals");
            const int view_loc=shader.loc("uView"),projection_loc=shader.loc("uProjection");
            const int uv_loc=shader.loc("uUsarTextura");
            const int tm=terrainShader.loc("uModel"), tv=terrainShader.loc("uView");
            const int tnormal=terrainShader.loc("uNormalMatrix");
            const int tflat=terrainShader.loc("uFlatNormal"), tdebug=terrainShader.loc("uShowNormals");
            const int tp=terrainShader.loc("uProjection"), tt=terrainShader.loc("uTeselas");
            const int sv=skyShader.loc("uView"), sp=skyShader.loc("uProjection");
            const int so=skyShader.loc("uOjo"), sh=skyShader.loc("uAlturaSuelo");
            const int sm=skyShader.loc("uMetrosTesela");
            constexpr float metersPerTile=50.0f; // 10 km / 200 repeticiones
            const float tiles=config.cells*config.spacing/metersPerTile;
            terrainShader.set_uniform(tm,glm::mat4(1.0f));
            terrainShader.set_uniform(tnormal,glm::mat3(1.0f));
            terrainShader.set_uniform(tt,tiles);
            skyShader.set_uniform(sh,0.0f);
            skyShader.set_uniform(sm,metersPerTile);
            std::cout<<"Textura de suelo: "<<tiles<<" repeticiones, "<<metersPerTile<<" m/tesela\n";
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            std::cout << "SPACE pausa | F alambre | T prueba UV | ESC salir | Mouse orbita/zoom\n";
            std::cout << "N normales | B normales planas terreno | L luz | K sol bajo | H niebla | G gamma\n";
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
                // Cada programa usa SUS propios uniform, aunque compartan nombres.
                send_lighting(skyShader,ctx,camera.data().eye,fogSRGB);
                send_lighting(terrainShader,ctx,camera.data().eye,fogSRGB);
                send_lighting(shader,ctx,camera.data().eye,fogSRGB);
                terrainShader.set_uniform(tflat,ctx.flatNormals ? 1:0);
                terrainShader.set_uniform(tdebug,ctx.showNormals ? 1:0);
                shader.set_uniform(solidDebug,ctx.showNormals ? 1:0);

                // A) Cubemap + suelo continuado: fondo primero SIN escribir profundidad.
                if(ctx.wireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
                glDepthMask(GL_FALSE);
                glDisable(GL_CULL_FACE); // desde dentro del cubo sus caras son traseras
                skyShader.use();
                skyShader.set_uniform(sv,glm::mat4(glm::mat3(camera.data().view)));
                skyShader.set_uniform(sp,camera.data().projection);
                skyShader.set_uniform(so,camera.data().eye);
                skyTexture.bind(0);
                groundTexture.bind(1);
                glBindVertexArray(skyCube.vao());
                glDrawElements(GL_TRIANGLES,skyCube.count(),GL_UNSIGNED_INT,nullptr);
                glDepthMask(GL_TRUE);
                glEnable(GL_CULL_FACE);
                if(ctx.wireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

                // B) Terreno texturado: las UV crecen de 0 a 200 teselas.
                terrainShader.use();
                terrainShader.set_uniform(tv,camera.data().view);
                terrainShader.set_uniform(tp,camera.data().projection);
                terrainShader.set_uniform(tt,ctx.showUVTest ? 1.0f : tiles);
                if(ctx.showUVTest) uvTestTexture.bind(0);
                else groundTexture.bind(0);
                glBindVertexArray(terrain.vao());
                glDrawElements(GL_TRIANGLES,terrain.count(),GL_UNSIGNED_INT,nullptr);

                // C) Avion: colores originales o una textura asimetrica (tecla T).
                shader.use();
                uvTestTexture.bind(0);
                shader.set_uniform(uv_loc,ctx.showUVTest ? 1:0);
                for(const auto& item:items){
                    const glm::mat4 M=glm::scale(item.model,glm::vec3(4.0f));
                    const glm::mat3 G=glm::transpose(glm::inverse(glm::mat3(M)));
                    shader.set_uniform(model_loc,M);
                    shader.set_uniform(normalMat,G);
                    shader.set_uniform(matKd,item.material.kd);
                    shader.set_uniform(matKs,item.material.ks);
                    shader.set_uniform(matNs,item.material.ns);
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
