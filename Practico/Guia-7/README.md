# CGyAV - Guia 07: Terreno procedural

## Objetivo

Continuar la Guia 06, incorporando **malla poligonal indexada** con relieve procedural, normales por vertice, test de profundidad y descarte de caras traseras. Conserva el FDM, la aeronave, la camara y los controles previos.

## Ejecutar (Ubuntu 24.04)

```bash
cd Guia-7
# Solo si aun no tenes dependencias: ./preparar-dependencias.sh
make clean && make
make run
```

Abrir desde `Guia-7` porque `ResourceManager` resuelve `assets/` relativamente al directorio actual. Controles: flechas, A/D y W/S como en la Guia 06; `ESPACIO` pausa, `F` alterna relleno y alambre, mouse izquierdo orbita, derecho acerca/aleja y `ESC` sale.

## Que se implemento y donde

1. **`src/modules/Terrain.h`**: `TerrainConfig` configura N=400 celdas/lado, `spacing=25 m`, `amplitude=150 m`, `frequency=0.01`, semilla 2026. `Alturas` representa la fuente de alturas separada de la malla.
2. **`src/modules/Terrain.cpp`**: `make_terrain` genera `(N+1)^2=160.801` vertices y `2*N^2=320.000` triangulos. Los indices usan el orden `{a,d,c,a,c,b}` para que las caras visibles desde arriba sean **antihorarias (CCW)**. Las normales se calculan por diferencias centrales de la funcion de altura, con su componente Y siempre positiva.
3. **`src/main.cpp`**: crea una unica malla GPU para el terreno, imprime las alturas minima y maxima, activa `GL_DEPTH_TEST` y `GL_CULL_FACE`, dibuja el terreno y luego la aeronave. El `FDM` parte de 5000 m reales, pero se representa una **ventana local** con altura visual inicial de 100 m para poder apreciar el terreno sin alterar la dinamica del vuelo.
4. **`src/modules/CameraSystem.*`**: amplian la profundidad de proyeccion para ver kilometros en lugar de solo 100 unidades. `InputHandler.cpp` incrementa la sensibilidad del zoom para esta escala.

La funcion de ruido implementa **Perlin 2D con cinco octavas fBm**, la misma tecnica conceptual que el ejemplo de la guia. Se implementa localmente para no exigir un header extra; el docente propone `FastNoiseLite` como opcion de biblioteca, no como requisito de API. Si la catedra exige exactamente FastNoiseLite, solo hay que sustituir `Alturas::operator()` y conservar `make_terrain`.

### Decisiones registradas

- **Ventana de terreno**: 400 celdas * 25 m = **10.000 m por lado**.
- **Alturas**: ruido Perlin fBm, semilla estable. La altura real obtenida se imprime al inicio: la amplitud es nominal y no implica que necesariamente se alcancen ambos extremos.
- **Escala avion**: las dimensiones del modelo grafico se multiplican por 4 para representar aproximadamente 8 m (no se modifica el FDM, que sigue usando metros).
- **Borde**: si el avion se acerca a 200 m del borde, el simulador **se pausa y avisa**. Para reiniciar en el centro, cerrar y volver a ejecutar.
- **Normales del borde**: como el ruido se puede evaluar tambien fuera de la grilla, las diferencias centradas son validas incluso en el borde (es la misma idea de muestrear alturas vecinas, sin normal cero).

## Lo que debes entender

**¿Por que el bucle de vertices va hasta N y el de celdas solo hasta N-1?** Porque N celdas tienen N+1 limites. `i<=N` crea el ultimo vertice; `i<N` recorre N celdas sin acceder a un vecino inexistente.

**¿Para que sirven los indices?** Cada esquina compartida de la grilla guarda una sola tupla `(posicion, normal, UV)` y varios triangulos referencian su indice. N=400 da 160.801 vertices vs. 960.000 referencias sin indexar.

**¿Por que puede desaparecer todo el terreno con CULL_FACE?** Si las caras fueron listadas en sentido horario vistas desde arriba, OpenGL las interpreta como traseras. El recorte se basa en el orden de los triangulos, **no** en el vector normal. Se corrige el orden de indices en el generador de malla.

**¿Que hace la normal?** Se aproxima a `(-df/dx, 1, -df/dz)`; se almacena ahora para usarla en la Guia 09 durante la iluminacion.

## Pruebas

1. `F` activa el modo alambre: observar el mallado rectangular y las diagonales de triangulacion.
2. Revertir con `F`: el relieve debe verse completo desde arriba. Si aparecen agujeros, revisar `indices` y `glFrontFace(GL_CCW)`.
3. Si modificas `cells` o `spacing`, comprobar que cambien los recuentos y las dimensiones de la ventana; la geometria se crea **una sola vez**, no cada cuadro.

La interfaz principal y las funciones exactas son variantes admitidas por el enunciado; no se anadio un sistema de LOD, terreno infinito ni simulacion de colisiones, porque no hacen falta para aprobar esta guia.
