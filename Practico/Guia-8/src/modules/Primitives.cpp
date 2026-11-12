#include "Primitives.h"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <array>
#include <cmath>
#include <stdexcept>
namespace primitives {
namespace {
void check_params(float radius, unsigned slices, unsigned rings) {
    if (!std::isfinite(radius) || radius <= 0.0f || slices < 3 || rings < 1)
        throw std::invalid_argument("Radio positivo, gajos >= 3 y anillos >= 1 requeridos");
}
void add_cap(MeshData& m, float y, float radius, unsigned slices, bool upper) {
    const unsigned center = static_cast<unsigned>(m.vertices.size());
    const float ny = upper ? 1.0f : -1.0f;
    m.vertices.push_back({{0,y,0},{0,ny,0},{.5f,.5f}});
    for (unsigned i = 0; i <= slices; ++i) {
        const float a = glm::two_pi<float>() * static_cast<float>(i) / slices;
        const float x = std::cos(a), z = std::sin(a);
        m.vertices.push_back({{radius*x,y,radius*z},{0,ny,0},{(x+1)*.5f,(z+1)*.5f}});
    }
    for (unsigned i = 0; i < slices; ++i) {
        const unsigned p = center + 1 + i, q = p + 1;
        if (upper) m.indices.insert(m.indices.end(),{center,q,p});
        else m.indices.insert(m.indices.end(),{center,p,q});
    }
}
}
MeshData cube(float sx,float sy,float sz) {
    if (sx<=0 || sy<=0 || sz<=0) throw std::invalid_argument("Dimensiones del cubo positivas");
    MeshData m;
    const std::array<std::array<glm::vec3,4>,6> faces = {{
        {{{-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}}},
        {{{.5f,-.5f,-.5f},{-.5f,-.5f,-.5f},{-.5f,.5f,-.5f},{.5f,.5f,-.5f}}},
        {{{.5f,-.5f,.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{.5f,.5f,.5f}}},
        {{{-.5f,-.5f,-.5f},{-.5f,-.5f,.5f},{-.5f,.5f,.5f},{-.5f,.5f,-.5f}}},
        {{{-.5f,.5f,.5f},{.5f,.5f,.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f}}},
        {{{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,-.5f,.5f},{-.5f,-.5f,.5f}}}
    }};
    const std::array<glm::vec3,6> normals = {{{0,0,1},{0,0,-1},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0}}};
    const std::array<glm::vec2,4> uv = {{{0,0},{1,0},{1,1},{0,1}}};
    for (unsigned f=0; f<6; ++f) {
        for (unsigned j=0;j<4;++j) {
            auto p=faces[f][j];
            m.vertices.push_back({{p.x*sx,p.y*sy,p.z*sz},normals[f],uv[j]});
        }
        unsigned a=f*4;
        m.indices.insert(m.indices.end(),{a,a+1,a+2,a,a+2,a+3});
    }
    return m;
}
MeshData cylinder(float radius,float length,unsigned slices,unsigned rings) {
    check_params(radius,slices,rings);
    if (!std::isfinite(length) || length<=0) throw std::invalid_argument("Largo positivo requerido");
    MeshData m;
    // Cada anillo lateral tiene N+1 vertices para la costura de textura.
    for(unsigned j=0;j<=rings;++j) {
        const float t=static_cast<float>(j)/rings;
        const float y=(t-.5f)*length;
        for(unsigned i=0;i<=slices;++i) {
            const float u=static_cast<float>(i)/slices;
            const float a=u*glm::two_pi<float>();
            const float x=std::cos(a),z=std::sin(a);
            m.vertices.push_back({{radius*x,y,radius*z},{x,0,z},{u,t}});
        }
    }
    for(unsigned j=0;j<rings;++j) for(unsigned i=0;i<slices;++i) {
        const unsigned a=j*(slices+1)+i,b=a+1,c=a+(slices+1),d=c+1;
        m.indices.insert(m.indices.end(),{a,c,b,b,c,d});
    }
    add_cap(m,-length*.5f,radius,slices,false);
    add_cap(m,length*.5f,radius,slices,true);
    return m;
}
MeshData cone(float radius,float angle,unsigned slices,unsigned rings) {
    check_params(radius,slices,rings);
    if (!std::isfinite(angle) || angle<=0 || angle>=glm::half_pi<float>())
        throw std::invalid_argument("Conicidad en radianes: 0 < angulo < pi/2");
    const float height=radius/std::tan(angle);
    if (!std::isfinite(height)) throw std::invalid_argument("Altura no representable");
    MeshData m;
    for(unsigned j=0;j<=rings;++j) {
        const float t=static_cast<float>(j)/rings;
        const float y=(t-.5f)*height;
        const float ring_radius=radius*(1.0f-t);
        for(unsigned i=0;i<=slices;++i) {
            const float u=static_cast<float>(i)/slices;
            const float a=u*glm::two_pi<float>();
            const float x=std::cos(a),z=std::sin(a);
            // En el apice se duplican vertices para conservar normales por gajo y UV.
            const glm::vec3 normal=glm::normalize(glm::vec3(x,height>0?radius/height:0,z));
            m.vertices.push_back({{ring_radius*x,y,ring_radius*z},normal,{u,t}});
        }
    }
    for(unsigned j=0;j<rings;++j) for(unsigned i=0;i<slices;++i) {
        const unsigned a=j*(slices+1)+i,b=a+1,c=a+(slices+1),d=c+1;
        m.indices.insert(m.indices.end(),{a,c,b});
        // En el ultimo anillo el apice coincide geometricamente: evitar triangulo degenerado.
        if(j+1<rings) m.indices.insert(m.indices.end(),{b,c,d});
    }
    add_cap(m,-height*.5f,radius,slices,false);
    return m;
}
bool check_winding(const MeshData& m) {
    for(std::size_t i=0;i<m.indices.size();i+=3) {
        const auto& a=m.vertices[m.indices[i]];
        const auto& b=m.vertices[m.indices[i+1]];
        const auto& c=m.vertices[m.indices[i+2]];
        const glm::vec3 cross=glm::cross(b.position-a.position,c.position-a.position);
        if(glm::dot(cross,cross)<1e-12f) continue; // triangulo degenerado
        const glm::vec3 n=(a.normal+b.normal+c.normal)/3.0f;
        if(glm::dot(cross,n)<=0) return false;
    }
    return true;
}
}
