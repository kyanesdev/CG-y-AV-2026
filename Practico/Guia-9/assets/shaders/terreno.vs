#version 460 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform float uTeselas;
out vec3 vPosMundo;
out vec3 vNormalMundo;
out vec2 vTexCoord;
void main(){
    vec4 p=uModel*vec4(aPos,1.0);
    vPosMundo=p.xyz;
    vNormalMundo=uNormalMatrix*aNormal;
    vTexCoord=uTeselas*aTexCoord;
    gl_Position=uProjection*uView*p;
}
