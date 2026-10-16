#pragma once
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
private:
 unsigned int id_=0;
};
