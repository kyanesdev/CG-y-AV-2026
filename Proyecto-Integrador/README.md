# AeroIUA: Mateando en las nubes — v0.6

Proyecto integrador de Computacion Grafica y Ambientes Virtuales, CRUC-IUA (Cordoba). C++17 + OpenGL/GLFW/GLM con las geometrías y el FDM `dlfdm` suministrado por la catedra. Se parte de la Guia 6 y se conserva la arquitectura modular.

## Primera version estable (v0.6)

- **W/S con respuesta mas marcada:** conservamos la palanca de potencia rapida de v0.5 y ajustamos *solo los parametros de la instancia del avion en el videojuego*: el empuje maximo y los coeficientes de arrastre `CD0`/`CDa` se multiplican por `2.0` a la vez. Al conservar la relacion empuje/arrastre se conserva aproximadamente el vuelo estabilizado del trim de 150 m/s a 5000 m. Como la fuerza neta cambia mas cuando se mueve W/S, la aceleracion y la desaceleracion se perciben antes. El valor es una **adaptacion didactica/arcade**, no la ficha tecnica real del entrenador. Las ecuaciones del FDM original no cambian en esta version.
- **No se inyecta velocidad artificial** ni se cambia directamente `body_velocity`. La velocidad sigue siendo calculada por el FDM usando fuerzas y masa. En `SIM 0.6X` todo transcurre mas despacio que en `SIM 1X`. Reducir potencia no equivale a frenar instantaneamente.
- **Sin cambios** de controles laterales, camaras, zoom, menu, HUD, checkpoints, maniobras, limite del mapa ni contacto con el suelo.
- **Git:** se agrega `.gitignore` para evitar subir `build/`, `bin/`, salidas y objetos. Conservar `src/`, `assets/`, `libs/`, `third_party/`, `tests/`, `Makefile*` y este README.

### Validacion recomendada

1. Desde el menu, JUGAR con modo `SIM 1X` (tecla T), nivelar el avion.
2. Dejar POT cerca de 50%, observar `VEL` y `ACC` durante unos segundos.
3. Mantener W hasta `POT 100%` y observar VEL/ACC durante 8-10 s; seleccionar S hasta `POT 0%` y medir 8-10 s. Comparar contra v0.5 en condiciones similares.
4. Revisar que la aeronave no acelera notablemente sola al comenzar en trim, que las flechas funcionan y que los 5 aros y la puntuacion no se alteraron.
5. Test unitario de parametros (si hay GLM instalado): `g++ -std=c++17 -Ilibs/dlfdm/include tests/test_school_settings.cpp libs/dlfdm/src/dlfdm/models/aircraft/jettrainer.cpp -o /tmp/aero-settings-test && /tmp/aero-settings-test`.

### Versionado en Git

Desde la raiz del repositorio CGyAV:

```bash
git add Proyecto-Integrador/
git status --short
git commit -m "feat(aeroiua): primera version estable de escuela de vuelo"
# Elegir la rama/remoto segun el repositorio que ya tengas configurado.
git push
```

**Nota:** si los binarios `build/` o `bin/` ya estaban versionados, `.gitignore` no los quita del indice. Revisar con `git ls-files Proyecto-Integrador/build Proyecto-Integrador/bin` y desvincularlos con `git rm --cached` antes del commit, sin borrarlos del disco.

## Historial anterior (v0.5)

## Cambios respecto de v0.4

1. **Alerones intermedios:** mando habitual de `0.080 rad` (aprox. 4,6 grados), entre el valor demasiado suave de v0.4 (`0.035`) y el mando acrobatico (`0.12`). Vuelven gradualmente a cero al soltar. Elevador habitual `0.065 rad` respecto al trim.
2. **W/S mas responsivos:** la palanca pasa de 0 a 100% en aproximadamente `0.83 s` al mantener la tecla. El FDM sigue calculando la velocidad: un avion no cambia de 540 a 300 km/h en un instante. El HUD incorpora `ACC +/-N KMH/S`, que indica aceleracion/desaceleracion real del modelo, suavizada cada actualizacion. Prueba W/S durante al menos 8-12 segundos en vuelo nivelado, con `T` en tiempo 1x si se desea ver cambios mas deprisa. El motor, las fuerzas y el trim no fueron retocados en este parche.
3. **Menu contextual:** antes de iniciar dice `JUGAR`; tras empezar, `CONTINUAR`. `REINICIAR` es la unica forma de reiniciar: se retiro la tecla `R`. `Esc` durante el vuelo abre menu y pausa, `Esc` en menu sale.
4. **Contacto con el relieve (primera etapa de colisiones):** si la posicion del avion cae a menos de 2 m sobre el suelo, se pausa la simulacion, se indica `SUELO - MENU` y se solicita `Esc` > `REINICIAR`. No hay rebotes, explosiones ni modificaciones de fuerzas. Si se supera el limite horizontal de vuelo, tambien es obligatorio reiniciar desde menu.
5. **Se conserva v0.4:** tres camaras, zoom, terreno esquematico con edificios, los cinco aros, detector de tonel/looping, puntaje y el modo escuela `T`.

## Controles

| Teclas | Funcion |
|---|---|
| Flechas | Alerones y elevador (autocentrado) |
| Shift + flechas | Mando amplio para maniobras |
| A / D | Timon |
| W / S | Aumentar / disminuir potencia del motor (revisar `POT` y `ACC`) |
| T | Cambiar ritmo 0.6x / 1x sin alterar velocidad fisica del modelo |
| 1 / 2 / 3 | Camaras cabina, seguimiento y cinematografica |
| 3 nuevamente | Cambiar angulo cinematografico suavemente |
| Mouse derecho arrastrar | Zoom de seguimiento hacia/desde cola |
| Mouse izquierdo arrastrar | Orbitar seguimiento |
| P o espacio | Pausa / continuar (solo si no hubo contacto ni salida del mapa) |
| Esc durante vuelo | Abrir menu y pausar |
| Flechas + Enter en menu | Jugar/Continuar, Reiniciar, Salir |
| Esc en menu | Salir del programa |

**No se usa R para reiniciar.** Cuando la partida termina por contacto con suelo o salida del mapa, seleccionar `REINICIAR` en menu, no `CONTINUAR`.

## Compilar y ejecutar

```bash
cd /home/kyanesdevops/Facultad/CGyAV/Proyecto-Integrador
make clean
make -j"$(nproc)"
./bin/aeroiua
```

El equipo debe disponer de compilador C++17, GLM, GLFW y bibliotecas OpenGL correspondientes. `glad` y `dlfdm` estan incluidos en el proyecto.

## Pruebas manuales importantes

1. En el primer arranque, el menu dice JUGAR. Seleccionar JUGAR, volar unos segundos, pulsar Esc: ahora dice CONTINUAR; Enter regresa sin borrar puntos.
2. Probar izquierda/derecha con una pulsacion corta, soltar y verificar que deja de aumentar la tasa de alabeo. Shift+flechas sigue siendo mas fuerte.
3. Con `T` en `SIM 1X`, mantener W hasta POT 100%, dejar volar 10 segundos; observar `VEL` y `ACC`. Despues mantener S hasta POT 0%, repetir. No se espera cambio instantaneo de velocidad por la inercia del FDM.
4. Reiniciar solo usando Esc > Reiniciar. Verificar que el conteo de aros y maniobras vuelve a cero.
5. Descender hacia el terreno: comprobar que aparece SUELO - MENU y el avion deja de actualizarse; Esc > Continuar no debe reanudar y Esc > Reiniciar si.
6. Comprobar 3 camaras y 5 aros igual que v0.4.

## Cumplimiento / limitaciones

- Escenario 3D, avion con actitud, HUD con al menos tres indicadores, FDM + teclado, 3 camaras, 5 aros ordenados y puntos estan implementados.
- Detector de tonel y detector de looping: **que el detector este presente no demuestra que el reactor complete un looping fisico**. Validarlo pilotando, sin otorgar puntos arbitrariamente.
- El FDM comienza con trim fisico a 5000 m y aproximadamente 150 m/s; una transformacion de origen lo representa a 120 m sobre el escenario. El modo 0.6x ralentiza el tiempo **simulado**, no la velocidad aerodinamica.
- El campus del IUA sigue siendo **esquematico** hasta contar con imagen satelital cenital contrastable. No presentar coordenadas como levantamiento topografico exacto.
- El contacto con suelo no incluye colision detallada con cada edificio ni dinamica de aterrizaje; es el paso minimo de deteccion de limites del escenario.
- Ademas del juego funcional, el trabajo necesita documentacion y explicacion tecnica para la defensa. Declarar adecuadamente la asistencia de IA.
