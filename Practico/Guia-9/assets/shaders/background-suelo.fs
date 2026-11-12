in vec3 vDireccion;
layout(binding=0) uniform samplerCube uCielo;
layout(binding=1) uniform sampler2D uTexturaSuelo;
uniform float uAlturaSuelo;
uniform float uMetrosTesela;
out vec4 FragColor;
void main(){
    vec3 d=normalize(vDireccion);
    float t=(uOjo.y-uAlturaSuelo)/max(-d.y,1.0e-4);
    vec2 xz=uOjo.xz+t*d.xz;
    vec3 P=vec3(xz.x,uAlturaSuelo,xz.y);
    Material m;
    m.kd=a_lineal(texture(uTexturaSuelo,xz/uMetrosTesela).rgb);
    m.ks=vec3(0.0); m.ns=1.0;
    vec3 suelo=aplicar_niebla(iluminar(P,vec3(0,1,0),m),P);
    vec3 cielo=a_lineal(texture(uCielo,d).rgb);
    // El cielo real no se ilumina otra vez: solo fusiona el horizonte con la niebla.
    float horizon=(uFogEnabled!=0)?1.0-smoothstep(0.0,0.10,d.y):0.0;
    cielo=mix(cielo,a_lineal(uNieblaColor),horizon);
    vec3 color=(d.y<0.0)?suelo:cielo;
    FragColor=vec4(a_srgb(color),1.0);
}
