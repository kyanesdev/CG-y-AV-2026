# AeroIUA: Mateando en las nubes — v0.3

Trabajo integrador de Computación Gráfica y Ambientes Virtuales (CRUC-IUA). Deriva de la Guía 6 y conserva el **FDM original `dlfdm` de la cátedra sin modificar**.

## Implementado

- Entorno OpenGL C++17 / GLFW / GLM con mallas generadas manualmente: terreno ondulado de 10 × 10 km, cielo, caminos, vegetación, edificios y fachada reconocible del Rectorado.
- Una aeronave basada en cubos, cilindros y conos reutilizados. Agrega tomas de aire, franjas, punteras, deriva y escape. No utiliza modelos externos.
- Cámara 1 en cabina, cámara 2 de persecución y cámara 3 **cinematográfica** con campo visual más estrecho. En la tercera, cada toma se mantiene fija en su posición y observa al avión; se inicia otra toma fija cuando el avión se aleja demasiado o al pulsar nuevamente 3.
- HUD operativo: velocidad, altura relativa al terreno (AGL), alabeo/horizonte y puntuación.
- Cinco checkpoints en orden (+100 puntos c/u), detección de cruce mediante segmento contra plano del aro.
- Detección de tonel (+250 puntos) a través de variación acumulada del roll del FDM con controles de continuidad, rumbo y altitud.
- Detección de looping (+250 puntos) a través de **trayectoria real** en un plano vertical. Se exige una rotación completa de la dirección de vuelo (no simplemente sostener una flecha).
- Menú simple totalmente propio en OpenGL: Jugar / Reiniciar / Salir.
- Pausa, reinicio, límite horizontal del área y paso de simulación fijo a 120 Hz.

## Limitación importante sobre el looping

El código **original** `libs/dlfdm/src/dlfdm/fdmsolver.cpp` restringe `theta` a [-1.5,+1.5] radianes. Esta protección numérica de los ángulos de Euler significa que un **looping convencional de 360° de cabeceo no está garantizado y probablemente no sea realizable con ese solver**. Hemos implementado un detector fiel a la trayectoria, pero **no se debe dar por validado el requisito práctico de looping hasta poder ejecutar la maniobra con el FDM**. Tampoco corresponde sumar puntos falsamente por mantener el elevador. En la siguiente fase habrá que comprobar el alcance del modelo y evaluar con la cátedra una solución segura para la representación de actitud.

## Fidelidad del campus

Se tomó como referencia visual la fachada del IUA que proporcionó el alumno: ingreso simétrico, ventanas, techo rojizo, escalones, mástil. El conjunto de edificios, árboles, caminos y escalas sigue siendo **esquemático**; NO se ha obtenido un plano cenital confiable del terreno. No afirmar que es una reconstrucción satelital exacta. Una captura cenital con orientación norte y escala permitirá adecuar las posiciones.

## Dependencias y compilación

Necesita Linux con GCC/g++ C++17, OpenGL 4.6, GLFW, GLM y X11. `glad` y `dlfdm` ya están dentro del proyecto.

```bash
sudo apt install build-essential libglfw3-dev libglm-dev libgl1-mesa-dev
make clean
make -j"$(nproc)"
./bin/aeroiua
```

**Al aplicar un parche ZIP, desde la raíz de `CGyAV` y no desde dentro del proyecto:**

```bash
unzip -o ~/Downloads/AeroIUA-v03-parche.zip -d Proyecto-Integrador
cd Proyecto-Integrador
make clean && make -j"$(nproc)" && ./bin/aeroiua
```

## Controles

| Control | Acción |
| --- | --- |
| Flechas y Enter en menú | Seleccionar Jugar / Reiniciar / Salir |
| Esc durante el vuelo | Volver al menú |
| Esc en el menú | Salir |
| 1 / 2 / 3 | Cámara cabina / seguimiento / cinematográfica |
| Pulsar nuevamente 3 | Nueva toma cinematográfica fija |
| Flechas ↑ ↓ | Cabeceo (elevador) |
| Flechas ← → | Alabeo (alerones) |
| A / D | Timón |
| W / S | Potencia |
| P o espacio | Pausa |
| R | Reinicio directo |
| F | Wireframe (depuración) |

Se conserva el mouse para ajustar vistas (heredado Guía 6), pero todo se puede usar solo con teclado.

## Puntuación

- Cinco checkpoints: 5 × 100 = 500.
- Tonel real detectado: 250.
- Looping real detectado: 250.
- Máximo previsto: 1000. La única manera de alcanzar esa puntuación es comprobar físicamente las dos maniobras.

## Pruebas manuales

1. Compilar desde cero; la salida debe ser `bin/aeroiua`.
2. El menú aparece al iniciar. Flechas arriba/abajo cambian resaltado. Enter en **Jugar** inicia.
3. Cambiar 1 / 2 / 3: todas muestran el avión o la cabina sin errores. En 3 el avión debe verse más grande; la cámara cambia de posición fija cuando se aleja.
4. Pulsar Esc: pausa y muestra menú; **Jugar** regresa, **Reiniciar** restaura trim, aros y puntos; **Salir** cierra.
5. La cámara 2 debe conservar el sentido correcto de las flechas validado en v0.1.
6. Observar las mejoras de avión y fachada al aproximarse al campus.
7. Atravesar cinco aros en orden: la puntuación del HUD sube de 100 en 100.
8. Intentar un tonel; solo sumará si el FDM realiza 360° reales y la trayectoria cumple los filtros.
9. Verificar el HUD `LOOP 0/1 TON 0/1` y no dar por finalizado el looping por acumulación ficticia de tiempo.
10. Probar R, P y el límite de vuelo.

### Prueba sintética de la lógica del detector (no sustituye la prueba de vuelo)

```bash
g++ -std=c++17 -Ilibs/dlfdm/include tests/test_maneuvers.cpp src/modules/Maneuvers.cpp -o /tmp/aeroiua-test
/tmp/aeroiua-test
```

La prueba sintética verifica que el algoritmo detecta una vuelta de 360°, **no** que el entrenador del FDM tenga capacidad aerodinámica para realizarla.

## Arquitectura resumida

- `main.cpp`: ciclo, GLFW, estados del menú, FDM a paso fijo, HUD y render.
- `CameraSystem.*`: vista/proyección y cortes cinematográficos.
- `Aircraft.*`: geometría de aeronave y cabina; mallas compartidas.
- `Scene.*`: grilla y campus esquemático.
- `Course.*`: checkpoints y orden.
- `Maneuvers.*`: validación de tonel/looping con estados reales.
- `Hud.*`: fuente raster de 5×7 hecha con triángulos y menú/HUD en OpenGL.

Las mejoras extra evaluables por la cátedra quedan para una etapa posterior; en esta versión el menú no se presenta como extra ya acreditado sin consultar al docente.
