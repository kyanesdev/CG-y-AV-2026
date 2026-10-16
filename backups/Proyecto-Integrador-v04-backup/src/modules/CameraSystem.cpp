#include "CameraSystem.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace {
glm::mat4 attitude(const glm::vec3& a) {
    glm::mat4 r(1.0f);
    r=glm::rotate(r,a.y,{0,1,0});
    r=glm::rotate(r,a.z,{0,0,1});
    return glm::rotate(r,a.x,{1,0,0});
}
glm::vec3 rotated(const glm::mat4& m,const glm::vec3& v) {
    return glm::vec3(m*glm::vec4(v,0.0f));
}
constexpr glm::vec3 shot_offsets[] = {
    {13.0f,4.2f,12.0f},  // frontal costado
    {-5.0f,3.8f,-18.0f}, // lateral opuesto
    {-17.0f,5.0f,10.0f} // trasera diagonal
};
}

CameraSystem::CameraSystem(int width,int height) {
    set_viewport(width,height);
    update({0,120,0},{0,0,0},CameraCommand{});
}
void CameraSystem::set_viewport(int width,int height) {
    if(width<=0 || height<=0) return;
    viewport_width_=width; viewport_height_=height;
    const float aspect=static_cast<float>(width)/static_cast<float>(height);
    const float fov=mode_==CameraMode::Flyby ? 37.0f : 66.0f;
    data_.projection=glm::perspective(glm::radians(fov),aspect,0.08f,12000.0f);
}
void CameraSystem::set_mode(CameraMode mode,const glm::vec3& position,
                            const glm::vec3& angles) {
    const bool another_shot=(mode==CameraMode::Flyby && mode_==CameraMode::Flyby);
    mode_=mode;
    if(mode_==CameraMode::Flyby) {
        if(another_shot) shot_index_=(shot_index_+1)%3;
        else shot_index_=0;
        // La direccion del encuadre permanece fija respecto al mundo hasta
        // pedir otra toma; el ojo sigue la traslacion del avion sin teletransporte.
        locked_yaw_=angles.y;
        const auto R=glm::rotate(glm::mat4(1.0f),locked_yaw_,{0,1,0});
        const glm::vec3 desired=rotated(R,shot_offsets[shot_index_]);
        if(!another_shot) smoothed_offset_=desired;
    }
    (void)position;
    set_viewport(viewport_width_,viewport_height_);
}
void CameraSystem::update(const glm::vec3& position,const glm::vec3& angles,
                          const CameraCommand& cmd,float dt) {
    const glm::mat4 full=attitude(angles);
    if(mode_==CameraMode::Cockpit) {
        const glm::vec3 eye=position+rotated(full,{0.10f,0.42f,0.0f});
        const glm::vec3 fwd=rotated(full,{1.0f,0.0f,0.0f});
        const glm::vec3 up=rotated(full,{0.0f,1.0f,0.0f});
        data_.view=glm::lookAt(eye,eye+fwd,up);
    } else if(mode_==CameraMode::Chase) {
        chase_distance_=std::clamp(chase_distance_+cmd.dist_delta,5.0f,38.0f);
        orbit_yaw_+=cmd.yaw_delta;
        orbit_pitch_=std::clamp(orbit_pitch_+cmd.pitch_delta*4.0f,-1.0f,3.0f);
        const auto yaw=glm::rotate(glm::mat4(1.0f),angles.y+orbit_yaw_,{0,1,0});
        // El zoom acerca hacia la COLA: la altura se reduce con la distancia.
        const float height=std::clamp(0.22f*chase_distance_+orbit_pitch_,0.85f,10.0f);
        const glm::vec3 eye=position+rotated(yaw,{-chase_distance_,height,0.0f});
        data_.view=glm::lookAt(eye,position+glm::vec3(0,0.45f,0),{0,1,0});
    } else {
        const auto R=glm::rotate(glm::mat4(1.0f),locked_yaw_,{0,1,0});
        const glm::vec3 desired=rotated(R,shot_offsets[shot_index_]);
        const float blend=1.0f-std::exp(-3.2f*std::clamp(dt,0.0f,0.1f));
        smoothed_offset_+=blend*(desired-smoothed_offset_);
        // Toma de angulo fijo con travelling: no se recoloca a saltos cada 115m.
        const glm::vec3 eye=position+smoothed_offset_;
        data_.view=glm::lookAt(eye,position,{0,1,0});
    }
}
