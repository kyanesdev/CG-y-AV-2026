#version 460 core
in vec3 vNormal;
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    //FragColor = vec4(uColor, 1.0);
    // Para depurar normales, reemplazar la linea anterior por:
    FragColor = vec4(vNormal * 0.5 + 0.5, 1.0);
}
