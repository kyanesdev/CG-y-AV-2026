#include <cassert>
#include <cmath>
#include <iostream>
#include <glm/gtc/constants.hpp>
#include "../src/modules/Maneuvers.h"

int main() {
    // Prueba determinista del DETECTOR, no prueba del alcance acrobatico del FDM.
    constexpr float dt=0.01f;
    Maneuvers m;
    dlfdm::AircraftState before{}, after{};
    before.body_velocity=glm::vec3(150.f,0.f,0.f);
    after.body_velocity=before.body_velocity;
    for(int i=0;i<850;++i) {
        after.phi=before.phi+0.85f*dt;
        after.body_omega.x=0.85f;
        m.update(before,after,{0,120,0},{1.5f,120,0},dt);
        before=after;
    }
    assert(m.tonel_done());
    assert(m.score()==250);
    m.reset();
    assert(!m.tonel_done() && !m.looping_done());

    // Trayectoria circular sintética 3D: vuelo en plano vertical con una vuelta.
    const float radius=200.f;
    const float omega=0.75f;
    before={}; after={};
    before.body_velocity=glm::vec3(150.f,0.f,0.f);
    after.body_velocity=before.body_velocity;
    after.body_omega.y=0.75f;
    for(int i=0;i<930;++i) {
        const float a=i*omega*dt, b=(i+1)*omega*dt;
        glm::vec3 p{radius*std::sin(a),220.f+radius*(1-std::cos(a)),0};
        glm::vec3 q{radius*std::sin(b),220.f+radius*(1-std::cos(b)),0};
        m.update(before,after,p,q,dt);
    }
    assert(m.looping_done());
    assert(m.score()==250);
    std::cout << "Tests sinteticos del detector OK (tonel y looping)\n";
}
