# Guía rápida para el parcial práctico (offline)

## Los 6 pasos de una escena — recordar el orden

1. `camera.look_at(ojo, objetivo, arriba, fov)`; por ejemplo `glm::vec3(0,3,8)`.
2. `scene.set_background(glm::vec3(...))` (un color) o dos `vec3` (horizonte y cenit).
3. Crear **materiales** con `std::make_shared<Tipo>()` y modificar sus propiedades.
4. Añadir **objetos** con `scene.add_object(std::make_shared<Figura>(..., material))`.
5. Añadir **luces** con `scene.add_light(Light(pos, intensidad, atenuacion))`.
6. `scene.set_max_depth(8U)` para limitar la recursión.

Todo se define dentro de `void build_scene_XX(Scene& scene, Camera& camera)`. Archivos en `src/escenas/`. Usá `#include <memory>` y las cabeceras de los objetos/materiales utilizados.

## Coordenadas y colores

- **X** derecha/izquierda, **Y** arriba/abajo, **Z** profundidad (según la posición y orientación de la cámara).
- En escenas habituales un suelo horizontal está en `y=0` con normal `glm::vec3(0,1,0)`.
- Los colores son `(rojo, verde, azul)` **lineales**, cada componente normalmente entre 0 y 1, por ejemplo rojo `(0.8, 0.1, 0.1)`, verde `(0.1, 0.8, 0.1)`, azul `(0.1, 0.1, 0.8)`; blanco `(1,1,1)`. **Las luces pueden superar 1**.
- `glm::vec3(0.5f)` es el atajo de `(0.5f, 0.5f, 0.5f)`.
- La posición de una esfera apoyada con radio `r` sobre suelo `y=0` suele tener centro `y=r`.
- Cambiar **ojo** desplaza la cámara; cambiar **objetivo** cambia hacia dónde mira; modificar **FOV** cambia cuánto se abarca. No confundir cámara con objeto.

## Figuras, copiar / cambiar coordenadas

**Esfera**: centro `(x,y,z)`, radio `r`, material `mat`:

```cpp
scene.add_object(std::make_shared<Sphere>(glm::vec3(0.0f,1.0f,0.0f),1.0f,mat));
```

**Plano infinito**: punto sobre el plano, normal, material:

```cpp
scene.add_object(std::make_shared<Plane>(glm::vec3(0,0,0),glm::vec3(0,1,0),mat));
```

**Rectángulo**: dos triángulos compartiendo vértices (los cuatro puntos en el mismo plano):

```cpp
const glm::vec3 a(-2,0,-2), b(2,0,-2), c(2,0,2), d(-2,0,2);
scene.add_object(std::make_shared<Triangle>(a,b,c,mat));
scene.add_object(std::make_shared<Triangle>(a,c,d,mat));
```

## Materiales (no se crean con `new`; usamos shared_ptr)

**Opaco Phong** (`Diffuse`):

```cpp
auto rojo = std::make_shared<Diffuse>();
rojo->ka = glm::vec3(0.04f, 0.0f, 0.0f); // ambiental
rojo->kd = glm::vec3(0.80f, 0.10f, 0.10f); // difuso = color del objeto
rojo->ks = glm::vec3(0.40f); // intensidad de brillo especular
rojo->shininess = 40.0f; // mas alto = brillo mas concentrado
```

**Tablero** (`Checker`):

```cpp
auto tablero = std::make_shared<Checker>();
tablero->size=0.5f; // tamaño de casilla
tablero->ka=glm::vec3(0.05f);
tablero->kd=glm::vec3(0.85f); // casilla clara
tablero->ka2=glm::vec3(0.005f);
tablero->kd2=glm::vec3(0.08f,0.08f,0.3f); // casilla oscura
```

**Espejo** (`Mirror`):

```cpp
auto espejo = std::make_shared<Mirror>();
espejo->ka=glm::vec3(0.0f);
espejo->kd=glm::vec3(0.0f);
espejo->ks=glm::vec3(1.0f);
espejo->shininess=120.0f;
espejo->kr=0.85f; // peso del rayo reflejado; 0 no refleja
```

**Vidrio/agua** (`Dielectric`):

```cpp
auto vidrio = std::make_shared<Dielectric>();
vidrio->ior=1.5f; // vidrio macizo en aire
vidrio->transparency=0.8f; // transmision por SUPERFICIE para rayos de sombra
vidrio->ks=glm::vec3(1.0f);
vidrio->shininess=100.0f;
```

- Agua: `ior=1.33f`; vidrio: `ior=1.5f`; diamante: `ior=2.42f`.
- Aire en agua: `ior=1.0f/1.33f`; el índice es **relativo**, no índice absoluto negativo.
- Esfera hueca: dos esferas **concéntricas**, exterior radio `r` con `ior=1.5`; interior radio menor (ej. `r*0.96`) con `ior=1/1.5`. En la interior fijar `ks=glm::vec3(0)`.
- `transparency=0.8` deja pasar `0.8*0.8=0.64` por dos cruces del **rayo de sombra**. La proporción de reflexión/refracción visible la gobierna Fresnel mediante `ior`, **no** `transparency`.
- `kr` representa reflexión del espejo; `ks` el **destello** local de una luz puntual, no una imagen reflejada.

## Luces

```cpp
const glm::vec3 att(1.0f,0.12f,0.02f);
scene.add_light(Light(glm::vec3(0,6,4),glm::vec3(3,3,3),att));
```

- Posición de luz y color de luz son diferentes de posición y color del objeto.
- Una esfera bloquea una luz y proyecta una sombra; varias luces se evalúan **por separado**.
- Sin luces sólo se ve ambiente y rayos que reflejan/refractan el entorno (depende del material).
- Atenuación: `1/(1 + 0.12*d + 0.02*d*d)`, donde `d` es distancia a la luz.

## Compilación inmediata

```bash
make clean && make -j2
./bin/rrt -l
./bin/rrt 12 -r 640 360 -ss 1 -o prueba.png
./bin/rrt 12 -r 1280 720 -ss 3 -o entrega.png
```

Modificar una escena ya existente -> `make -j2` suele bastar; si hay dudas -> `make clean && make -j2`.

## Errores típicos que debés reconocer

| Síntoma | Causa probable |
|---|---|
| `No existe la escena 13` | Falta registrarla en `escenas.cpp`, o número incorrecto |
| `build_scene_13 was not declared` | Falta declarar función en `escenas.h` |
| `undefined reference to build_scene_13` | Declaración/definición no coinciden o no hay archivo con función |
| `glm/glm.hpp: No such file` | GLM no instalada en la máquina; **no es fallo de la escena** |
| Imagen toda del fondo | Cámara apunta fuera de los objetos; escena no contiene objetos visibles |
| Suelo negro | Falta luz, difuso casi negro, sombras o materiales mal asignados |
| Vidrio demasiado oscuro | Profundidad insuficiente; revisar índices de esfera hueca |
| Esfera transparente con sombra negra | Material no dieléctrico, `transparency=0` o motor sin paso 10 |
| Muchos reflejos oscuros | `kr<1`, caída por rebotes; o recursión cortada |
| PNG no aparece | Verificar nombre `-o`, directorio actual y permisos |

## Guion corto para la interpretación escrita

> En la escena hay **[objetos y posiciones]**, con materiales **[tipos y parámetros]** e iluminación **[luces y colores]**. En la imagen se observa **[sombra, brillo, inversión, reflejo, cambio de intensidad]** porque **[rayos de sombra / reflexión `kr` / refracción `ior` / Fresnel / recursión]**. Si se modifica **[parámetro]**, se espera **[efecto concreto]**. Una limitación del trazador es **[por ejemplo sombras transparentes no se refractan; sólo se atenúan]**.

No memorizarlo sin entender: cada causa debe ser compatible con lo que la imagen realmente muestra.
