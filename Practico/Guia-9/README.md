# CGyAV - Guia 09: Iluminacion, niebla y correccion gamma

## Ejecutar

```bash
cd Guia-9
./preparar-dependencias.sh  # una sola vez si no se instalo desde Guia-8
make clean && make
make run
```

## Cambios respecto a Guia 08

1. **`assets/shaders/iluminacion.glsl`**: una unica implementacion compartida de `Material`, **Phong**, niebla exponencial y funciones de conversion sRGB/lineal. No lleva `#version`.
2. **`Shader.cpp`**: el compilador de fragment shaders acepta **tres cadenas** mediante `glShaderSource`: `#version 460 core`, codigo comun y codigo particular. Los fragment shaders no repiten funciones.
3. **`terreno.vs/.fs`**: pasa posicion y normal a coordenadas de mundo; el color de la textura deja de ser resultado directo y se vuelve `kd` del material; la superficie es **mate** (`ks = 0`).
4. **`solid.vs/.fs`**: ilumina las piezas con sus coeficientes `kd`, `ks` y `ns`; `Aircraft` produce un `Material` por pieza. La **matriz normal** se calcula en CPU como `transpose(inverse(mat3(M)))`, porque las piezas tienen escalados no uniformes.
5. **`background-suelo.fs`**: ilumina el plano ficticio con normal constante `(0,1,0)`, pero **no vuelve a iluminar el cielo** del cubemap. La niebla une el suelo lejano con el horizonte.
6. **`src/main.cpp`**: estima el color de niebla promediando la fila central de las cuatro caras laterales del cielo. Envía al terreno, al avion y al fondo los parametros compartidos **por programa** (cada uno tiene uniform propios).

## Parametros escogidos y justificacion

- Ventana: **10.000 m** de lado, N=400, separacion 25 m.
- Textura: **200 teselas de 50 m** por lado.
- Iluminacion: luz ambiente `0.30`, sol `0.70`, direccional, con intensidades en espacio lineal. Asi, una normal mirando al sol tiene aporte total cercano a 1.
- Material terreno: `ks=0`, sin brillo de plastico. Aeronave: `kd` segun el color de cada pieza, `ks=0.20`, `ns=32`, brillo moderado.
- **Visibilidad V=2.000 m**: a 2 km el color de niebla participa aproximadamente un 63%; a 4 km, un 86%; a 5 km (borde del terreno desde el centro), un 92%. Esta eleccion disimula la transicion entre relieve y suelo plano.
- Niebla: `f = 1 - exp(-d/V)`; la mezcla se calcula **despues** de la iluminacion.
- Gamma: `a_lineal(c) = c^2.2` en los colores de entrada sRGB y `a_srgb(c) = c^(1/2.2)` al final. No activar tambien `GL_FRAMEBUFFER_SRGB`, porque se aplicaria dos veces la conversion.

## Controles para comprobar la teoria

| Tecla | Resultado |
|---|---|
| `L` | Luz Phong encendida/apagada (conserva textura) |
| `N` | Visualizacion de normales como colores |
| `B` | Sustituye normales del terreno por `(0,1,0)` |
| `K` | Sol alto / sol bajo (comparar relieve) |
| `H` | Niebla encendida/apagada |
| `G` | Correccion gamma encendida/apagada |
| `T` | Prueba de UV en el terreno y las piezas de la aeronave |
| `F` | Malla de alambre / relleno |
| `ESPACIO` | Pausa del FDM |
| `ESC` | Salir |

## Preguntas que hay que poder contestar

**Phong:** calcula ambiente + difusa (`kd max(dot(N,L),0)`) + especular (`ks pow(max(dot(R,V),0),ns)`). La iluminacion es por fragmento: el shader normaliza la normal interpolada de los vertices.

**¿Por que no alcanza usar la matriz M para las normales?** Los escalados no uniformes pueden inclinar incorrectamente una normal. La matriz inversa transpuesta conserva su perpendicularidad con la superficie.

**¿Por que todas las piezas responden al mismo sol?** Comparten los mismos parametros de escena, pero cada shader recibe sus propios uniform. El material pertenece a cada pieza, no a la escena.

**¿Por que el color de niebla coincide con el cielo?** Un gris fijo haria aparecer una banda en el horizonte. Se calcula un promedio de los pixeles de las filas de horizonte del cubemap para evitarla.

**¿Por que se necesita gamma?** Las sumas de iluminacion deben realizarse con intensidades lineales. Las texturas y colores elegidos a ojo suelen representar sRGB. Se convierten al entrar y se convierten otra vez al salir.

**¿Por que no hay sombras proyectadas por el sol en este practico?** Phong calcula el aporte local de luz pero no resuelve la visibilidad entre los objetos y la luz; **sombras mediante rayos** es otro concepto, visto en el trazador de rayos de la Guia 10.

## Verificaciones exigidas por la guia

- `N`: las normales del terreno y de las piezas deben tener colores coherentes con su orientacion.
- `B`: al reemplazar normales del terreno por `(0,1,0)` desaparece gran parte del relieve aparente, aun cuando la geometria sigue siendo la misma.
- `K`: con el sol bajo se distinguen mas las laderas orientadas hacia la luz.
- `H`: con niebla la transicion a suelo continuado debe quedar disimulada; sin ella, el borde puede notarse.
- `G`: la transicion entre luz y sombra cambia, pero **el cielo debe conservar aproximadamente sus colores originales**.

**Limitacion de prueba:** los shaders y la integracion OpenGL quedan listos, pero la ejecucion grafica requiere una computadora con GLFW/GLM/OpenGL 4.6 y un display. El paquete se baso en el codigo de Guia 06 adjunto y los recursos oficiales de la catedra.
