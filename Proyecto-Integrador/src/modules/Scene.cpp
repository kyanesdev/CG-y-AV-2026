#include "Scene.h"
#include "Primitives.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

float Scene::terrain_height(float x, float z) {
    // Relieve suave y region central casi plana para el campus.
    const float dist = std::sqrt(x*x + (z+1000.0f)*(z+1000.0f));
    const float weight = std::clamp((dist-370.0f)/700.0f, 0.0f, 1.0f);
    return weight * (11.0f*std::sin(x*0.0017f)*std::cos(z*0.0015f)
                     + 6.0f*std::sin(z*0.0028f));
}

void Scene::add_cube(glm::vec3 center, glm::vec3 size, glm::vec3 color) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), center);
    m = glm::scale(m, size);
    pieces_.push_back({0,m,color});
}

void Scene::add_tree(float x, float z) {
    const float ground = terrain_height(x,z);
    add_cube({x,ground+3.2f,z}, {1.4f,6.4f,1.4f}, {0.36f,0.28f,0.15f});
    glm::mat4 m = glm::translate(glm::mat4(1.0f), {x,ground+11.5f,z});
    m = glm::scale(m, {15.0f,22.0f,15.0f});
    pieces_.push_back({1,m,{0.17f,0.42f,0.23f}});
}

void Scene::init() {
    // Grilla 10 x 10 km; sin mapas externos ni librerias adicionales.
    constexpr unsigned count = 121;
    constexpr float side = 10000.0f;
    const float step = side/static_cast<float>(count-1);
    MeshData mesh;
    mesh.vertices.reserve(count*count);
    mesh.indices.reserve((count-1)*(count-1)*6);
    for (unsigned iz=0; iz<count; ++iz) {
        const float z = -side/2 + iz*step;
        for (unsigned ix=0; ix<count; ++ix) {
            const float x = -side/2 + ix*step;
            const float h = terrain_height(x,z);
            const float dx = (terrain_height(x+2,z)-terrain_height(x-2,z))/4;
            const float dz = (terrain_height(x,z+2)-terrain_height(x,z-2))/4;
            mesh.vertices.push_back({{x,h,z},glm::normalize(glm::vec3(-dx,1,-dz)),{0,0}});
        }
    }
    for (unsigned iz=0; iz<count-1; ++iz) for (unsigned ix=0; ix<count-1; ++ix) {
        const unsigned a = iz*count+ix;
        const unsigned b = a+1, c=a+count, d=c+1;
        mesh.indices.insert(mesh.indices.end(),{a,c,b,b,c,d});
    }
    terrain_.load(mesh);
    cube_.load(primitives::cube());
    cone_.load(primitives::cone(0.5f, glm::radians(29.0f), 12));

    // Campus ESQUEMATICO: posiciones aproximadas, NO reconstruccion satelital.
    // Base central; edificios principales, secundarios, caminos y vegetacion.
    add_cube({0,0.4f,-1000}, {630,0.8f,610}, {0.40f,0.46f,0.40f});
    add_cube({0,0.9f,-770}, {23,1.0f,470}, {0.41f,0.42f,0.43f});
    add_cube({-160,1.0f,-1000}, {250,0.7f,15}, {0.41f,0.42f,0.43f});
    add_cube({135,1.0f,-1170}, {190,0.7f,13}, {0.41f,0.42f,0.43f});
    // Edificio blanco institucional con volumen central y techo rojizo.
    add_cube({0,10.5f,-910}, {110,20,80}, {0.90f,0.88f,0.78f});
    add_cube({0,21.0f,-910}, {117,2,87}, {0.67f,0.27f,0.19f});
    add_cube({0,25.0f,-911}, {25,8,28}, {0.89f,0.87f,0.79f});
    add_cube({-165,7.0f,-1030}, {110,13,135}, {0.82f,0.83f,0.76f});
    add_cube({-165,14.0f,-1030}, {114,1,140}, {0.48f,0.37f,0.30f});
    add_cube({173,9.0f,-1050}, {125,17,110}, {0.80f,0.81f,0.76f});
    add_cube({173,18.0f,-1050}, {129,1,115}, {0.49f,0.39f,0.31f});
    add_cube({140,7.0f,-1210}, {115,13,65}, {0.84f,0.84f,0.80f});
    add_cube({140,14.0f,-1210}, {119,1,69}, {0.58f,0.33f,0.29f});
    add_cube({-100,5.0f,-1190}, {100,9,62}, {0.80f,0.83f,0.81f});
    // Referencia VISUAL: fachada simetrica del Rectorado fotografiada por el alumno.
    // La huella urbana sigue siendo esquematica hasta tener plano cenital verificable.
    const glm::vec3 ivory{0.95f,0.94f,0.89f};
    const glm::vec3 glass{0.35f,0.43f,0.48f};
    // Frente del Rectorado, mirando hacia el acceso / calle frontal (+Z).
    for(float x : {-43.f,-28.f,28.f,43.f}) {
        add_cube({x,8.0f,-868.8f},{10.0f,12.0f,0.9f},ivory);
        add_cube({x,8.8f,-868.0f},{5.0f,5.6f,0.3f},glass);
        add_cube({x,15.6f,-868.0f},{5.0f,5.2f,0.3f},glass);
    }
    add_cube({0,11.0f,-868.2f},{19.0f,17.0f,1.3f},ivory);
    add_cube({0,5.9f,-867.1f},{10.5f,11.8f,0.5f},{0.34f,0.23f,0.15f});
    add_cube({0,12.0f,-866.7f},{1.0f,9.0f,0.5f},ivory);
    add_cube({0,24.0f,-871.0f},{39.0f,1.7f,4.0f},ivory);
    add_cube({0,28.0f,-871.0f},{27.0f,6.0f,3.6f},ivory);
    // Escalinata de acceso y mastil.
    for(int step=0;step<3;++step)
        add_cube({0,0.5f+step*0.35f,-850.0f-step*2.4f},
                 {27.0f-step*3.0f,0.5f+step*0.2f,7.5f},
                 {0.74f,0.73f,0.68f});
    add_cube({26,15,-838},{0.55f,30.0f,0.55f},{0.79f,0.79f,0.75f});
    add_cube({27.8f,26.0f,-838.0f},{4.0f,2.0f,0.25f},{0.37f,0.70f,0.92f});
    add_cube({27.8f,27.2f,-838.0f},{4.0f,0.9f,0.26f},ivory);
    // Cartel rectangular de ingreso, seto bajo y areas verdes, sin texturas.
    add_cube({0,2.0f,-810},{150.0f,3.8f,3.2f},{0.87f,0.85f,0.76f});
    for(int i=-7;i<=7;++i) {
        const float x = static_cast<float>(i)*17.0f;
        add_cube({x,1.5f,-823.0f},{10.0f,3.0f,5.0f},{0.19f,0.42f,0.19f});
    }
    // Patio, estacionamiento, veredas y bloques academicos secundarios.
    add_cube({-195,0.95f,-830},{140.0f,0.8f,85.0f},{0.38f,0.41f,0.42f});
    for(int i=0;i<7;++i)
        add_cube({-255.0f+i*17.0f,1.38f,-827.0f},{0.75f,0.06f,42.0f},{0.84f,0.83f,0.71f});
    add_cube({196,0.97f,-910},{12.0f,0.7f,220.0f},{0.62f,0.63f,0.60f});
    add_cube({-235,0.97f,-1040},{11.0f,0.7f,320.0f},{0.62f,0.63f,0.60f});
    add_cube({-40,8.5f,-1250},{92,16,45},{0.85f,0.86f,0.80f});
    add_cube({-40,17.0f,-1250},{96,1.0f,48},{0.43f,0.39f,0.35f});
    // Ventanas laterales que dan referencia de escala al sobrevolar.
    for(int i=0;i<5;++i) {
        add_cube({-210.0f+i*20.0f,6.0f,-961.6f},
                 {9.0f,6.0f,0.6f},glass);
        add_cube({120.0f+i*22.0f,7.0f,-994.4f},
                 {9.0f,7.0f,0.6f},glass);
    }
    for(float x : {-305.f,-260.f,-215.f,-170.f,-125.f,-80.f,80.f,125.f,170.f,215.f,260.f,305.f})
        add_tree(x,-715.0f);

    // Senderos + lineas de arbolado para reconocer un predio desde arriba.
    for (int i=0;i<14;++i) {
        const float z = -1290.0f + i*42.0f;
        add_tree(-285.0f,z);
        add_tree(290.0f,z);
    }
    for (int i=0;i<7;++i) {
        const float x = -120.0f + i*39.0f;
        add_tree(x,-1320.0f);
    }
}

void Scene::collect(std::vector<RenderItem>& out) const {
    out.push_back({&terrain_,glm::mat4(1.0f),{0.29f,0.49f,0.30f}});
    for (const auto& part : pieces_) {
        out.push_back({part.mesh == 0 ? &cube_ : &cone_, part.transform, part.color});
    }
}
