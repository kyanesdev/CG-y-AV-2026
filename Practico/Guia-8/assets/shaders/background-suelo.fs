#version 460 core
in vec3 vDireccion;
layout(binding=0) uniform samplerCube uCielo;
layout(binding=1) uniform sampler2D uTexturaSuelo;
uniform vec3 uOjo;
uniform float uAlturaSuelo;
uniform float uMetrosTesela;
out vec4 FragColor;
void main(){
    vec3 d=normalize(vDireccion);
    float t=(uOjo.y-uAlturaSuelo)/max(-d.y,1.0e-4);
    vec2 xz=uOjo.xz+t*d.xz;
    vec4 suelo=texture(uTexturaSuelo,xz/uMetrosTesela);
    vec4 cielo=texture(uCielo,vDireccion);
    FragColor=(d.y<0.0)?suelo:cielo;
}
