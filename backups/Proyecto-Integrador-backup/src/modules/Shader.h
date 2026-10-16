#pragma once
#include <glm/glm.hpp>
#include <string>
class Shader {
public:
 Shader()=default;
 ~Shader();
 Shader(const Shader&)=delete;
 Shader& operator=(const Shader&)=delete;
 Shader(Shader&& other) noexcept;
 Shader& operator=(Shader&& other) noexcept;
 bool compile_from_source(const std::string& vs,const std::string& fs);
 void use() const;
 void clear();
 unsigned int id() const { return id_; }
 // DSA: no requieren activar previamente el programa.
 int loc(const std::string& name) const;
 void set_uniform(const std::string& name,const glm::mat4& m) const;
 void set_uniform(const std::string& name,const glm::vec3& v) const;
 void set_uniform(int location,const glm::mat4& m) const;
 void set_uniform(int location,const glm::vec3& v) const;
private:
 unsigned int id_=0;
};
