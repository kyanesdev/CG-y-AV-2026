# AeroIUA: Mateando en las nubes — v0.2

Proyecto integrador de Computacion Grafica y Ambientes Virtuales. Base: Guia 6 del practico de la catedra.

## Alcance de esta etapa

- FDM `dlfdm` de la materia **sin modificar**: actualizacion fija a 120 Hz.
- Aeronave hecha con cubos, cilindros y conos. Conserva el alabeo con signo validado por el alumno.
- Tres camaras (teclas 1/2/3): cabina simple, seguimiento y cinematica desde punto fijo.
- Grilla de terreno 10 x 10 km, cielo y campus esquematico con edificios, senderos y vegetacion.
- HUD en OpenGL, con velocidad en km/h, altura sobre terreno en metros, alabeo y horizonte artificial.
- Cinco aros, atravesados **en orden** de sur a norte en coordenadas de escena (-Z).
- Cada aro cruzado suma 100 puntos; se muestra por pantalla y en terminal.
- Limite de area +/- 4900 m en X/Z, pausa y tecla R para reiniciar.

**IMPORTANTE:** esta version NO modela exactamente el campus real; es un primer bloque esquematico. Tampoco implementa por ahora el menu Jugar/Reiniciar/Salir ni la deteccion de looping y tonel. Se agregaran tras validar el flujo actual. Los indicadores circulares de la cabina son decorativos; los instrumentos dinamicos reales estan en el HUD.

## Dependencias

Linux, g++ con C++17, OpenGL 4.6, GLFW y GLM (GLAD y dlfdm vienen en el repositorio).

En Debian/Ubuntu, si hiciera falta instalar paquetes: `sudo apt install build-essential libglfw3-dev libglm-dev libgl1-mesa-dev`

## Compilar y ejecutar

Desde **Proyecto-Integrador/**:

```bash
make clean
make -j"$(nproc)"
./bin/aeroiua
```

## Controles

| Tecla | Accion |
|-------|--------|
| 1 | Camara de cabina |
| 2 | Camara de seguimiento (predeterminada) |
| 3 | Camara cinematica: pulsar de nuevo para recolocar |
| Flechas arriba/abajo | Elevador |
| Flechas izquierda/derecha | Alerones |
| A / D | Timon |
| W / S | Potencia |
| P o Espacio | Pausa |
| R | Reiniciar circuito y restaurar estado trim |
| F | Modo wireframe (diagnostico) |
| Esc | Salir |

Se puede jugar solo con teclado. Se conserva la entrada de mouse de la Guia 6, pero no es necesaria.

## Pruebas basicas

1. Abrir: debe verse el avion por detras, con suelo, cielo, edificios y aros.
2. Pulsar 1: primera persona. Deben verse tablero oscuro, relojes y marco de cabina.
3. Pulsar 2: tercera persona, con avion completo y horizonte sin inclinarse durante el roll.
4. Pulsar 3: punto de camara fijo que gira para mirar el avion. Pulsar 3 otra vez para generar nueva posicion fija.
5. Confirmar que izquierda inclina a la izquierda y derecha a la derecha (correccion validada).
6. Observar HUD: VEL, ALT, ROL varian al volar; ARO empieza en 0/5, PTS en 0.
7. Volar a traves del aro **amarillo** (a unos 480 m al norte) y revisar la terminal: +100 puntos y ARO 1/5.
8. Cruzar un aro celeste fuera de orden: NO debe sumar puntos.
9. Pulsar R: avion vuelve al punto inicial y ARO/PTS retornan a 0.
10. Volar mas alla de 4.9 km de coordenada horizontal X o Z: la simulacion se pausa; R reinicia.

## Diseno tecnico para explicar en la defensa

- `src/main.cpp`: inicializacion, ciclo de simulacion, reset, render 3D y 2D.
- `src/modules/CameraSystem.*`: matrices view y projection; transformacion por yaw/pitch/roll.
- `src/modules/Scene.*`: grilla indexada de vertices, relieve y bloques simples.
- `src/modules/Course.*`: aros hechos con segmentos de cubos; interseccion entre trayectoria y plano.
- `src/modules/Hud.*`: fuente pixelada y horizonte 2D, proyectados con `glm::ortho`.
- `src/modules/Aircraft.*`: mallas compartidas, matriz de transformacion local y cabina.
- `src/modules/InputHandler.*`: eventos y control continuo (sin cambios).
- `libs/dlfdm`: modulo oficial del practico (sin cambios).

## Escala del terreno respecto del FDM

El FDM inicia a 5000 m de altitud real segun NED, mientras el mundo grafico utiliza un origen relativo: `y = 120 - (z_NED - z_NED_inicial)`. Asi el avion aparece a 120 m sobre el suelo **sin alterar** la altitud fisica ni el integrador. Se mantienen las coordenadas norte/eje -Z y este/eje +X.

## Limitaciones y siguientes pasos

- La compilacion no pudo probarse en el entorno de generacion porque carece de `libglm-dev` y `libglfw3-dev`. Probar en el Linux del alumno y traer cualquier error textual.
- Los cinco aros funcionan como circuito preliminar, todavia falta disenar el recorrido final alrededor del campus.
- Looping y tonel deben evaluarse usando la actitud y velocidades angulares del FDM, sin crear movimiento artificial.
- Proximas iteraciones: validar visualmente, afinar camara/cabina, implementar maniobras y menu, mejorar campus con captura satelital real.

Este codigo es un apoyo de desarrollo con IA y debe declararse su uso segun la consigna del proyecto integrador.
