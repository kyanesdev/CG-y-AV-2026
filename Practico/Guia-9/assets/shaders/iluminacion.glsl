// Codigo comun de GLSL. La version viene ANTES desde Shader.cpp.
uniform vec3 uSolDir;        // desde la superficie hacia el sol, normalizada
uniform vec3 uSolColor;      // intensidad lineal del sol
uniform vec3 uAmbiente;      // intensidad lineal ambiente
uniform vec3 uOjo;           // camara, coordenadas de mundo
uniform vec3 uNieblaColor;   // color del horizonte sRGB
uniform float uVisibilidad;  // distancia d=V: 63% niebla
uniform int uFogEnabled;
uniform int uGammaEnabled;
uniform int uLightingEnabled;
struct Material {
    vec3 kd;
    vec3 ks;
    float ns;
};
const float GAMMA=2.2;
vec3 a_lineal(vec3 c) {
    return (uGammaEnabled!=0) ? pow(max(c,vec3(0.0)),vec3(GAMMA)) : c;
}
vec3 a_srgb(vec3 c) {
    return (uGammaEnabled!=0) ? pow(max(c,vec3(0.0)),vec3(1.0/GAMMA)) : c;
}
vec3 iluminar(vec3 P,vec3 N,Material m) {
    if(uLightingEnabled==0) return m.kd;
    vec3 L=normalize(uSolDir);
    vec3 V=normalize(uOjo-P);
    float NdotL=max(dot(normalize(N),L),0.0);
    vec3 ambiente=uAmbiente*m.kd;
    vec3 difusa=uSolColor*m.kd*NdotL;
    vec3 especular=vec3(0.0);
    if(NdotL>0.0){
        vec3 R=reflect(-L,normalize(N));
        especular=uSolColor*m.ks*pow(max(dot(R,V),0.0),m.ns);
    }
    return ambiente+difusa+especular;
}
vec3 aplicar_niebla(vec3 color,vec3 P) {
    if(uFogEnabled==0) return color;
    float d=length(P-uOjo);
    float f=1.0-exp(-d/max(uVisibilidad,0.01));
    return mix(color,a_lineal(uNieblaColor),clamp(f,0.0,1.0));
}
