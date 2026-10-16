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
