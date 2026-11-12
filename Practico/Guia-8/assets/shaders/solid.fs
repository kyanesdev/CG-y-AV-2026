#version 460 core
in vec2 vTexCoord;
layout(binding=0) uniform sampler2D uTextura;
uniform vec3 uColor;
uniform int uUsarTextura;
out vec4 FragColor;
void main(){
    FragColor=(uUsarTextura!=0)?texture(uTextura,vTexCoord):vec4(uColor,1.0);
}
