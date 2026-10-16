#version 460 core
in vec3 vNormal;
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    float sun = max(dot(normalize(vNormal), normalize(vec3(0.3,1.0,0.5))), 0.0);
    FragColor = vec4(uColor * (0.73 + 0.27 * sun), 1.0);
}
