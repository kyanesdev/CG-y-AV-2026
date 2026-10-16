#include "Primitives.h"
#include <array>
namespace primitives {
MeshData cube(float sx,float sy,float sz){
 MeshData m;
 // Cada fila: 4 vertices en orden antihorario vistos desde afuera.
 const std::array<std::array<std::array<float,3>,4>,6> faces = {{
  {{{-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}}},  // +Z
  {{{.5f,-.5f,-.5f},{-.5f,-.5f,-.5f},{-.5f,.5f,-.5f},{.5f,.5f,-.5f}}}, // -Z
  {{{.5f,-.5f,.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{.5f,.5f,.5f}}}, // +X
  {{{-.5f,-.5f,-.5f},{-.5f,-.5f,.5f},{-.5f,.5f,.5f},{-.5f,.5f,-.5f}}}, // -X
  {{{-.5f,.5f,.5f},{.5f,.5f,.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f}}}, // +Y
  {{{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,-.5f,.5f},{-.5f,-.5f,.5f}}} // -Y
 }};
 const std::array<std::array<float,3>,6> colors = {{{{1,0.3f,0.3f}},{{0.3f,1,0.3f}},{{0.3f,0.5f,1}},{{1,1,0.3f}},{{1,0.4f,1}},{{0.3f,1,1}}}};
 for(unsigned int face=0;face<6;++face){
  for(const auto& v:faces[face]) m.vertices.push_back({v[0]*sx,v[1]*sy,v[2]*sz,colors[face][0],colors[face][1],colors[face][2]});
  unsigned int i=face*4;
  m.indices.insert(m.indices.end(),{i,i+1,i+2,i,i+2,i+3});
 }
 return m;
}
}
