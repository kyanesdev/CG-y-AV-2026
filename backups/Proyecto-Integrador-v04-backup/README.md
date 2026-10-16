# AeroIUA: Mateando en las nubes — v0.4 (ajuste de vuelo)

Proyecto integrador de Computación Gráfica y Ambientes Virtuales (CRUC-IUA). Se parte de v0.3, manteniendo las tres cámaras, menú, aeronave, campus esquemático, HUD, cinco checkpoints y puntuación.

## Cambios de v0.4

1. **Control suave y autocentrado** (`InputHandler`): los alerones, elevador y timón alcanzan una deflexión pequeña al mantener pulsadas las teclas y vuelven gradualmente a neutro/trim cuando se sueltan. Se eliminó la acumulación indefinida que hacía incontrolable el alabeo. `Shift + flechas` aumenta temporalmente la autoridad para intentar acrobacias.
2. **Potencia visible**: `W/S` ahora ajustan la palanca aproximadamente un 40% por segundo. El HUD muestra `POT` en porcentaje. **Cambiar la potencia NO modifica instantáneamente la velocidad:** sigue siendo el FDM de la cátedra el que calcula la aceleración del reactor.
3. **Modo escuela**: por defecto corre a **0.6x de tiempo simulado**. La tecla `T` alterna entre 0.6x y 1x. Se mantienen el paso numérico fijo del FDM (1/120 s), todas sus velocidades calculadas y las ecuaciones aerodinámicas. No es un nuevo avión ni se falsean los km/h; solo se reproduce el vuelo a un ritmo más amable en tiempo real.
4. **Cámara 2 y zoom**: la altura sobre el avión depende de la distancia, de modo que al acercar con botón derecho la cámara se aproxima hacia la cola y no termina directamente encima de ella.
5. **Cámara 3**: deja de teleportarse cada 115 metros. Es un travelling desde un ángulo fijo respecto al mundo que sigue la traslación del avión para que se vea de un tamaño legible. Pulsar `3` repetidamente pasa suavemente entre tres encuadres diagonales. **No es una cámara inmóvil en el terreno:** una cámara realmente fija quedaría lejos del reactor en menos de un segundo.
6. **Orientación cerca de 90 grados** (`libs/dlfdm`): corrección mínima en la integración de orientación: se usa un cuaternión con las mismas velocidades angulares del FDM; para las ecuaciones de fuerzas y la salida se convierte a ángulos de Euler equivalentes. Se quita el bloqueo artificial `theta` a ±1.5 rad. Esto modifica SOLO el integrador de actitud del FDM, no sus modelos de aerodinámica, motor ni controles de trim. **No garantiza por sí solo que el reactor tenga energía o potencia de cabeceo suficiente para un looping real.** El detector conserva la exigencia de una trayectoria completa.
7. **HUD**: se muestra el ángulo de roll dentro de ±180° y el porcentaje de potencia y modo de tiempo. Tonel sigue detectándose por velocidad de roll efectiva, sin discontinuidades en ±180°.

## Controles

| Teclas | Función |
|---|---|
| 1 / 2 / 3 | Cabina / persecución / cinematográfica; pulsar 3 de nuevo cambia encuadre |
| Flechas | Alerones / elevador (mando escuela, suave) |
| Shift + flechas | Mayor autoridad de mando para acrobacias |
| A / D | Timón |
| W / S | Subir / bajar potencia del motor (ver `POT` en HUD) |
| T | Alternar tiempo escuela 0.6x y normal 1x |
| Mouse derecho (arrastrar) | Acercar / alejar cámara de persecución |
| Mouse izquierdo (arrastrar) | Orbitar cámara de persecución |
| P o espacio | Pausar / reanudar |
| R | Reiniciar vuelo y circuito |
| Esc | Volver al menú (Esc en menú: salir) |
| Flechas + Enter en menú | Jugar / Reiniciar / Salir |

## Instalación del parche

Desde `/home/kyanesdevops/Facultad/CGyAV`:

```bash
cp -a Proyecto-Integrador Proyecto-Integrador-v03-backup
unzip -o ~/Downloads/AeroIUA-v04-parche.zip -d Proyecto-Integrador
cd Proyecto-Integrador
make clean && make -j"$(nproc)"
./bin/aeroiua
```

## Pruebas prioritarias

- Volar recto sin mantener flechas; pulsar brevemente izquierda y soltar: la **superficie de alerón** debe volver a neutro y la rotación dejar de acelerarse, aunque el avión conserve algo de inclinación (normal en un avión). Repetir derecha y elevador.
- `W` mantenida durante 2 segundos: `POT` debe subir. Luego `S`: `POT` debe bajar. Los km/h cambian **gradualmente** con el empuje y resistencia, no igual que el porcentaje de mando.
- Pulsar `T` para comparar 0.6x y 1x. La velocidad HUD sigue expresando velocidad verdadera del FDM.
- Cámara 3: dejar volar 10 segundos y observar que no hay reinicios bruscos; volver a pulsar `3` para otro encuadre sin saltos de posición.
- Cámara 2: arrastrar clic derecho para acercarse a la cola sin subir excesivamente.
- Intentar un looping solo después de ganar mucha altura. `Shift+abajo` incrementa autoridad de elevador; el **FDM no garantiza** que se cierre el círculo con su modelo aerodinámico. Si pierde demasiada velocidad, recuperar el vuelo o pulsar R. **La corrección a los cuaterniones permite atravesar 90°, pero no genera empuje adicional.**
- Confirmar que 5 aros, menú, pausa, 3 vistas y reinicio conservan su comportamiento.

## Comprobaciones independientes (no sustituyen el juego)

```bash
python3 tests/test_orientation_math.py
# Si tu equipo tiene GLM instalado:
g++ -std=c++17 -Ilibs/dlfdm/include tests/test_maneuvers.cpp src/modules/Maneuvers.cpp -o /tmp/aeroiua-test
/tmp/aeroiua-test
```

**Estado de compilación:** este paquete no fue ejecutado gráficamente en el entorno de generación (faltaban GLM/GLFW). El usuario debe validar `make` y pruebas en su Linux. La prueba matemática independiente verifica continuidad de una orientación cuaterniónica durante 360°, pero no sustituye una prueba real del reactor del FDM.

**Campus:** continúa siendo una representación simplificada de los edificios y calles del IUA; faltan imágenes satelitales adecuadas para asegurar distancias y orientaciones. Evitar presentar el modelo como topografía exacta.
