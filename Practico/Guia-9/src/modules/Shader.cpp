#include "Shader.h"
#include <glad/gl.h>
#include <iostream>
#include <string>
#include <utility>
#include <glm/gtc/type_ptr.hpp>
namespace {
GLuint compile(GLenum type,const std::string& source,const std::string& common=""){
 GLuint shader=glCreateShader(type);
 const char* ptr=source.c_str();
 if (common.empty()) {
  glShaderSource(shader,1,&ptr,nullptr);
 } else {
  // Guia 09: version primero, funciones comunes y codigo propio, en ese orden.
  const char* segments[]={"#version 460 core\n", common.c_str(), source.c_str()};
  glShaderSource(shader,3,segments,nullptr);
 }
 glCompileShader(shader);
 GLint ok=0;
 glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
 if(!ok){
  GLint n=0; glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&n);
  std::string log(static_cast<std::size_t>(n>0?n:1),'\0');
  glGetShaderInfoLog(shader,n,nullptr,log.data());
  std::cerr<<"Error de compilacion de shader: "<<log<<'\n';
  glDeleteShader(shader); return 0;
 }
 return shader;
}
}
Shader::~Shader(){ clear(); }
Shader::Shader(Shader&& other) noexcept : id_(std::exchange(other.id_,0)) {}
Shader& Shader::operator=(Shader&& other) noexcept {
 if(this!=&other){clear();id_=std::exchange(other.id_,0);} return *this;
}
void Shader::clear(){if(id_)glDeleteProgram(id_);id_=0;}
bool Shader::compile_from_source(const std::string& vs,const std::string& fs,const std::string& common){
 clear();
 GLuint v=compile(GL_VERTEX_SHADER,vs);
 if(!v)return false;
 GLuint f=compile(GL_FRAGMENT_SHADER,fs,common);
 if(!f){glDeleteShader(v);return false;}
 GLuint p=glCreateProgram();
 glAttachShader(p,v);glAttachShader(p,f);glLinkProgram(p);
 glDeleteShader(v);glDeleteShader(f);
 GLint ok=0;glGetProgramiv(p,GL_LINK_STATUS,&ok);
 if(!ok){
  GLint n=0;glGetProgramiv(p,GL_INFO_LOG_LENGTH,&n);
  std::string log(static_cast<std::size_t>(n>0?n:1),'\0');
  glGetProgramInfoLog(p,n,nullptr,log.data());
  std::cerr<<"Error de linkeo: "<<log<<'\n';
  glDeleteProgram(p);return false;
 }
 id_=p;return true;
}
void Shader::use() const { glUseProgram(id_); }

int Shader::loc(const std::string& name) const {
 const int location=glGetUniformLocation(id_,name.c_str());
 if(location<0) std::cerr<<"Aviso: uniform inactivo o inexistente: "<<name<<'\n';
 return location;
}
void Shader::set_uniform(const std::string& name,const glm::mat4& m) const {set_uniform(loc(name),m);}
void Shader::set_uniform(const std::string& name,const glm::vec3& v) const {set_uniform(loc(name),v);}
void Shader::set_uniform(int location,const glm::mat4& m) const {
 if(location>=0) glProgramUniformMatrix4fv(id_,location,1,GL_FALSE,glm::value_ptr(m));
}
void Shader::set_uniform(int location,const glm::vec3& v) const {
 if(location>=0) glProgramUniform3fv(id_,location,1,glm::value_ptr(v));
}

void Shader::set_uniform(int location,float value) const {
 if(location>=0) glProgramUniform1f(id_,location,value);
}
void Shader::set_uniform(int location,int value) const {
 if(location>=0) glProgramUniform1i(id_,location,value);
}
void Shader::set_uniform(int location,const glm::mat3& m) const {
 if(location>=0) glProgramUniformMatrix3fv(id_,location,1,GL_FALSE,glm::value_ptr(m));
}
