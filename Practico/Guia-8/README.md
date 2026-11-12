# CGyAV - Guia 08: Texturas y cielo

## Ejecutar

```bash
cd Guia-8
./preparar-dependencias.sh  # una sola vez, incluye stb_image.h
make clean && make
make run
```

La instalacion de `libstb-dev` y la copia a `third_party/stb/stb_image.h` es intencional: la consigna propone usar `stb_image.h` para decodificar PNG. **No es necesario programar un cargador de PNG**. Los archivos oficiales `grass.png`, `sand.png` y las seis caras del cielo estan incluidos en este proyecto, en `assets/`.

## Que se implemento

- **`ResourceManager.h/.cpp`**: `ImageData` contiene imagen en RAM/CPU, siempre con **4 canales RGBA** (`stbi_load(...,4)`). `load_image(key,file,flip)` maneja lectura, cache, conflictos de clave y volteo vertical. `STB_IMAGE_IMPLEMENTATION` aparece **una sola vez**, en `ResourceManager.cpp`.
- **`Texture.h/.cpp`**: administra el recurso OpenGL/GPU con las mismas reglas que `Mesh`: destructor libera, copia prohibida, movimiento permitido. `load_2d` ejecuta **crear, reservar, cargar, parametrizar**, genera todos los mipmaps y usa `GL_REPEAT`. `load_cubemap` carga seis caras sin invertir, en orden `+X,-X,+Y,-Y,+Z,-Z`, con `GL_CLAMP_TO_EDGE`.
- **`assets/shaders/terreno.vs/.fs`**: usa `tex_coords` de cada vertice, las multiplica por `uTeselas` y obtiene el color de la textura (`sampler2D` binding 0).
- **`assets/shaders/background.vs` + `background-suelo.fs`**: el cubemap usa una **direccion 3D**, no UV. Se elimina la traslacion de la vista para que el cielo siga al observador. Hacia abajo, el shader calcula la interseccion de la direccion con el plano horizontal `y=0` y texturiza un suelo continuado.
- **`assets/shaders/solid.vs/.fs`**: `T` activa una imagen de prueba **asimetrica** sobre el terreno y las primitivas del avion; asi se verifican giros, inversiones y costuras UV.
- **`src/main.cpp`**: fondo primero con `glDepthMask(GL_FALSE)`, terreno y finalmente avion. Se reactiva `glDepthMask(GL_TRUE)` antes de dibujar el terreno.

## Parametros y formulas

- Ventana = **10 km x 10 km**.
- `uTeselas = 200` repeticiones sobre 10 km -> **50 metros por tesela**.
- `GL_LINEAR_MIPMAP_LINEAR` al reducir; `GL_LINEAR` al ampliar. Sin `glGenerateTextureMipmap`, los niveles reservados no tendrian contenido valido.
- Coordenadas de la malla: `UV = (j/N, i/N)`; escala en vertex shader `vUV = UV * 200`.
- Suelo continuo: si la direccion `d.y < 0`, interseccion con plano `y=0` en `t=(O.y-0)/(-d.y)`. Las UV salen del punto de interseccion `xz/50`, que coincide con la escala de la malla. Se toman **ambas texturas antes de elegir** suelo/cielo para conservar el calculo correcto de derivadas del mipmap.
- La textura del cielo se genera con las imagenes de la catedra; las seis caras deben tener **igual ancho y alto**.

## Conceptos para explicar en clase

**¿Texture y ResourceManager son lo mismo?** No. `ResourceManager` lee y decodifica archivos en CPU; `Texture` posee el identificador y la memoria de OpenGL en GPU.

**¿Por que se invierte la imagen del suelo pero no el cubemap?** La convencion de coordenadas de una textura 2D y de las caras del cubemap no es la misma. Si se invierte tambien el cubemap, aparece el cielo al reves o las uniones no coinciden.

**¿Por que la costura del cilindro no es un error?** El primer y el ultimo vertice lateral comparten posicion espacial pero tienen diferentes `u=0` y `u=1`, para que la textura se desenrolle correctamente.

**¿Por que el cielo no tapa los objetos?** Se dibuja primero, con escritura de profundidad desactivada; el terreno y el avion escriben posteriormente sus profundidades y lo tapan donde corresponde.

**¿Por que hace falta el suelo continuado?** Un terreno de extension finita deja una banda debajo del horizonte cuando la camara gana altitud. La interseccion rayo-plano rellena la zona sin necesidad de agrandar la malla.

## Verificacion

1. `T` permite observar `uv-test.png` orientada sobre el terreno y las piezas del avion.
2. Mover la camara: el cielo rota con la vista pero **no presenta paralaje** al trasladarla.
3. El fondo nunca debe ocultar el avion ni el terreno. Las texturas lejanas no deben parpadear: si ocurre, revisar mipmaps.
4. Para observar cambios en las teselas, editar **`metersPerTile` en `src/main.cpp`** y volver a compilar. Si se cambia solo `uTeselas` pero no `uMetrosTesela`, se ve una costura entre malla y suelo continuado.

En esta guia todavia no se aplica iluminacion, niebla ni correccion gamma. Eso corresponde al practico siguiente.
