in vec3 vPosMundo;
in vec3 vNormalMundo;
in vec2 vTexCoord;
layout(binding=0) uniform sampler2D uTextura;
uniform Material uMaterial;
uniform int uUsarTextura;
uniform int uShowNormals;
out vec4 FragColor;
void main(){
    vec3 N=normalize(vNormalMundo);
    if(uShowNormals!=0){ FragColor=vec4(N*0.5+0.5,1.0); return; }
    Material m=uMaterial;
    m.kd=a_lineal(m.kd);
    if(uUsarTextura!=0) m.kd*=a_lineal(texture(uTextura,vTexCoord).rgb);
    // El color original de la aeronave fue elegido visualmente: viene en sRGB.
    // Si la textura esta activa, el producto se efectua en lineal.
    vec3 color=iluminar(vPosMundo,N,m);
    color=aplicar_niebla(color,vPosMundo);
    FragColor=vec4(a_srgb(color),1.0);
}
