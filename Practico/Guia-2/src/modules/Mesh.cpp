#include "Mesh.h"
#include <glad/gl.h>
#include <cstddef>
#include <stdexcept>
#include <utility>
Mesh::~Mesh(){ clear(); }
Mesh::Mesh(Mesh&& other) noexcept { *this=std::move(other); }
Mesh& Mesh::operator=(Mesh&& other) noexcept {
 if(this!=&other){ clear(); vao_=std::exchange(other.vao_,0); vbo_=std::exchange(other.vbo_,0); ebo_=std::exchange(other.ebo_,0); count_=std::exchange(other.count_,0); }
 return *this;
}
void Mesh::clear(){
 if(ebo_) glDeleteBuffers(1,&ebo_);
 if(vbo_) glDeleteBuffers(1,&vbo_);
 if(vao_) glDeleteVertexArrays(1,&vao_);
 ebo_=vbo_=vao_=0; count_=0;
}
void Mesh::load(const MeshData& data){
 if(data.vertices.empty() || data.indices.empty() || data.indices.size()%3!=0) throw std::invalid_argument("Malla indexada invalida");
 for(auto i:data.indices) if(i>=data.vertices.size()) throw std::invalid_argument("Indice fuera de rango");
 clear();
 glGenVertexArrays(1,&vao_);
 glGenBuffers(1,&vbo_);
 glGenBuffers(1,&ebo_);
 glBindVertexArray(vao_);
 glBindBuffer(GL_ARRAY_BUFFER,vbo_);
 glBufferData(GL_ARRAY_BUFFER,data.vertices.size()*sizeof(Vertex),data.vertices.data(),GL_STATIC_DRAW);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER,data.indices.size()*sizeof(unsigned int),data.indices.data(),GL_STATIC_DRAW);
 glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex,px)));
 glEnableVertexAttribArray(0);
 glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex,r)));
 glEnableVertexAttribArray(1);
 glBindVertexArray(0);
 count_=static_cast<int>(data.indices.size());
}
