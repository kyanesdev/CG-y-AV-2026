// La version y el codigo compartido se agregan en Shader.cpp.
in vec3 vPosMundo;
in vec3 vNormalMundo;
in vec2 vTexCoord;
layout(binding=0) uniform sampler2D uTextura;
uniform int uFlatNormal;
uniform int uShowNormals;
out vec4 FragColor;
void main(){
    vec3 N=(uFlatNormal!=0)?vec3(0,1,0):normalize(vNormalMundo);
    if(uShowNormals!=0){ FragColor=vec4(N*0.5+0.5,1.0); return; }
    Material m;
    m.kd=a_lineal(texture(uTextura,vTexCoord).rgb);
    m.ks=vec3(0.0);  // tierra mate: sin reflejo especular
    m.ns=1.0;
    vec3 color=iluminar(vPosMundo,N,m);
    color=aplicar_niebla(color,vPosMundo);
    FragColor=vec4(a_srgb(color),1.0);
}
