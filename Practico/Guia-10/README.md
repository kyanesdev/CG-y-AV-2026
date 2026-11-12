# Guía 10 — Trazador de rayos recursivos (CGyAV 2026, CRUC-IUA)

Proyecto **listo para estudiar y practicar el segundo parcial práctico**. Parte del **esqueleto oficial de la cátedra**, incorpora las **soluciones oficiales de los talleres 1 y 2** y las **8 escenas de la guía 10**. Se conserva el motor de la materia, sin modificarlo para simular efectos nuevos.

## 1. Qué entra en el parcial práctico

Hay que escribir el `.cpp` de una escena nueva, compilarla, **generar un PNG** y **responder por escrito preguntas que interpreten esa imagen**. No se permite usar IA generativa durante la evaluación.

**El archivo que normalmente se modifica es `src/escenas/e12-plantilla-parcial.cpp`** (o se copia como base para una escena nueva). No hace falta cambiar el motor `src/rt/` para crear escenas con las primitivas y materiales disponibles.

## 2. Compilar y ejecutar, sin Internet

Requisitos del laboratorio: Ubuntu con `g++`, `make`, **GLM** (normalmente instalado como `libglm-dev`) y soporte de OpenMP. NO necesita OpenGL, GLFW, GLAD, GPU dedicada ni red para renderizar. Incluye `stb_image_write.h` dentro de `third_party/stb`.

Desde la carpeta que contiene **este** Makefile:

```bash
# 1. Compilar sin descargar nada
make clean && make -j2

# 2. Listar escenas disponibles (0, 1..8, 11, 12)
./bin/rrt -l

# 3. Render de prueba, rápido
./bin/rrt 12 -r 640 360 -ss 1 -o prueba.png

# 4. Render de entrega, de calidad (si el tiempo lo permite)
./bin/rrt 12 -r 1280 720 -ss 3 -o entrega.png

# 5. Abrir el PNG con el visor de Ubuntu (si está disponible)
xdg-open entrega.png
```

`-r W H` significa ancho y alto; `-ss 3` usa 3 × 3 = **9 rayos por píxel**. `-o` pone nombre al PNG de salida. `-n` genera una vista de **normales geométricas** para depuración. Sin `-o` el nombre predeterminado es `escena-NN.png`.

Para probar la guía 10: `./bin/rrt 1 -r 640 360 -ss 1`, luego reemplazar 1 por 2, 3, ... 8. La escena **8** es más costosa: para esa escena conviene usar baja resolución durante los ensayos.

Comprobación previa en Ubuntu **antes de desconectar Internet**:

```bash
g++ --version
make --version
test -f /usr/include/glm/glm.hpp && echo 'GLM disponible' || echo 'GLM NO disponible'
make clean && make -j2
./bin/rrt 12 -r 320 180 -ss 1 -o prueba.png
file prueba.png
```

> Si el compilador marca `fatal error: glm/glm.hpp: No such file or directory`, **falta GLM en ese equipo**. No se descarga automáticamente. Avisar al docente o usar una máquina del laboratorio que tenga la dependencia instalada, como verificó el profesor. El encabezado de GLM **no se incluye** en este paquete; sí se incluye el escritor PNG stb.

## 3. Archivos fundamentales

| Archivo | Para qué sirve | En el parcial |
|---|---|---|
| `src/escenas/e12-plantilla-parcial.cpp` | Modelo editable de cámara, fondo, plano, esferas y luz | **MODIFICAR** |
| `src/escenas/e01-...cpp` a `e08-...cpp` | Ejemplos completos de la guía 10 | Consultar / copiar bloques |
| `src/escenas/escenas.h` | Declara `build_scene_XX` | Agregar declaración si creás un nuevo número |
| `src/escenas/escenas.cpp` | Asocia números con funciones | Registrar nuevo número |
| `src/rt/scene.cpp` | Trazado recursivo, Phong, sombras, búsqueda de objetos | **NO tocar** |
| `src/rt/material.h`, `mirror.h`, `dielectric.cpp` | Materiales, reflexión, refracción | **NO tocar** |
| `src/rt/sphere.h`, `plane.h`, `triangle.h` | Intersecciones geométricas | **NO tocar** |
| `src/main.cpp` | Opciones y generación de PNG | **NO tocar** |

El `Makefile` detecta automáticamente los `.cpp` ubicados en `src/escenas/`.

## 4. Escenas resueltas

| ID | Escena | Fenómeno a poder explicar |
|---|---|---|
| 1 | Sombras de colores | Una sombra bloquea una luz, pero no necesariamente las demás |
| 2 | Espejos paralelos | Atenuación del brillo por `kr^n` y profundidad de recursión |
| 3 | Pasillo de espejos | Paredes triangulares finitas, espejo sobre esfera y brillo de Phong |
| 4 | Esferas de vidrio y agua | Refracción y transmisión parcial de sombras |
| 5 | Banco de índices | Cambio de dirección y reflectancia con `ior` |
| 6 | Maciza, hueca y burbuja | Dos superficies por cáscara; `ior < 1` de aire en agua |
| 7 | Piscina | Fresnel: más reflexión con mirada rasante |
| 8 | Matrioshka de vidrio | Árbol binario de rayos, corte de profundidad, alto costo |
| 11 | Whitted (taller 2) | Ejemplo original de espejo, vidrio y dos triángulos |
| 12 | Plantilla para practicar | Ejemplo reutilizable de piso + esfera opaca + vidrio + luz |

Imágenes generadas: `renders/`. Referencias: `referencias_docente/`. La guía original está en `consigna/`.

## 5. Crear una escena NUEVA de forma segura

Si el enunciado exige una escena adicional (por ejemplo número 13):

```bash
cp src/escenas/e12-plantilla-parcial.cpp src/escenas/e13-parcial.cpp
```

1. Abrir `src/escenas/e13-parcial.cpp` y **cambiar** `build_scene_12` por `build_scene_13`.
2. Cambiar cámara, fondo, objetos, colores, materiales, luces y profundidad **según el enunciado**.
3. En `src/escenas/escenas.h` agregar `void build_scene_13(Scene& scene, Camera& camera);`.
4. En `src/escenas/escenas.cpp`, dentro de `kScenes`, agregar `{ 13, "Mi escena de parcial", build_scene_13 },`.
5. Compilar `make -j2` y probar `./bin/rrt 13 -r 640 360 -ss 1 -o prueba13.png`.
6. Si coincide con el enunciado, generar el PNG de entrega.

**Alternativa más rápida:** editar directamente la escena **12** sin crear ni registrar otra función. Ya está en la tabla. Hacé una copia de seguridad antes si querés volver a la plantilla.

## 6. Documentación para estudiar (sin Internet)

- **`GUIA-RAPIDA-PARCIAL.md`**: copia y pegá con comprensión: cámara, figuras, materiales, luces, profundidad, errores frecuentes.
- **`INTERPRETACION-ESCENAS.md`**: preguntas resueltas de las 8 escenas, mecanismos físicos y limitaciones del modelo de Whitted.
- **`SIMULACRO-60-MINUTOS.md`**: consigna de práctica estilo parcial, para realizar sin ayuda de IA.

**Aclaración:** este proyecto genera PNG a partir de geometría; no implementa importación de archivos JPG/PNG como texturas. `Checker` es un patrón de casillas calculado por coordenadas. Si el examen pide "generar una imagen" significa crear el PNG renderizado, no cargar una foto externa.

## 7. Verificación

Se compiló el proyecto en un entorno aislado sin GLM instalada, mediante un reemplazo matemático mínimo **de pruebas que NO está en esta entrega**. Se renderizaron las 8 escenas y se compararon con las imágenes de la cátedra. A 1280 × 720 y `-ss 3`, las escenas 1, 2, 3, 5, 6 y 7 coincidieron píxel a píxel; la 4 tuvo diferencias numéricas mínimas por redondeo. La escena 8 se renderizó correctamente a 640 × 360 y `-ss 1`; la prueba de calidad máxima excedió los recursos de este entorno. **Antes del parcial corresponde recompilar el ZIP usando la GLM real en Ubuntu y realizar un render de prueba.**

Créditos: implementación sobre materiales de Diego Llorens / cátedra CGyAV, CRUC-IUA, año 2026. Material de estudio para fines académicos.
