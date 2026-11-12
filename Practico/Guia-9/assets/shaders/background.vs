#version 460 core
layout(location=0) in vec3 aPos;
uniform mat4 uProjection;
uniform mat4 uView; // sin traslacion
out vec3 vDireccion;
void main(){
    vDireccion=aPos;
    gl_Position=uProjection*uView*vec4(aPos,1.0);
}
