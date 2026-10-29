# 🕳️ AgujeroNegro — Ray Tracing por CPU

> **🌌 Un viaje visual por las inmediaciones de un agujero negro supermasivo**, inspirado en TON 618, creado píxel por píxel con C++17 y el procesador.

![Render del agujero negro](renders/AgujeroNegro.png)

**Materia:** Computación Gráfica y Ambientes Virtuales · Ingeniería Informática · CRUC–IUA · 2026  
**Autor:** Kevin Leonel Yanes

## 🎯 Objetivo

Generar una **imagen estática desde el espacio**, con un agujero negro en el centro, un disco de acreción luminoso y un campo estelar distorsionado. El foco de este ejercicio está en **comprender el trazado de rayos**, cómo una cámara virtual genera una imagen y cómo se determina qué contribuye a cada píxel.

La imagen es una **interpretación artística inspirada en TON 618**, no una fotografía de ese objeto ni un cálculo de sus parámetros físicos reales.

## 🧠 ¿Qué es ray tracing? ¿Es lo mismo que RTX?

- **Ray tracing (trazado de rayos)** es una técnica de renderizado que sigue rayos matemáticos a través de una escena para calcular visibilidad y color.
- **RTX** es una familia de tecnologías y hardware asociada principalmente a NVIDIA para acelerar, entre otras tareas, el ray tracing. **No se necesita una placa RTX para implementar el algoritmo.**
- **AMD Radeon Ray Tracing** es una familia de tecnologías y hardware asociada principalmente a AMD para acelerar, entre otras tareas, el ray tracing. **No se necesita una placa AMD RX para implementar el algoritmo.**
- En este proyecto, **todos los rayos se calculan con la CPU**. No se usa GPU, CUDA, shaders, OpenGL ni recursos descargados.

### 📚 Relación con la materia

Se aplican conceptos del material de la cátedra, especialmente **Unidad VII (proyección/pipeline de rendering)** y **Unidad VIII (determinación de superficies visibles; algoritmos basados en ray tracing)**:

| Concepto | Aplicación en este programa |
|---|---|
| Cámara y proyección perspectiva | Posición, dirección de vista, campo visual y relación de aspecto |
| Rayos y vectores 3D | Origen + dirección de cada píxel; producto escalar y vectorial |
| Muestreo de imagen | Uno o varios rayos por píxel, para suavizar los bordes |
| Intersecciones y visibilidad | Cruce con el plano del disco y detección del horizonte |
| Rendering | Conversión del color calculado en una imagen 2D |
| Color y sombreado | Emisión del disco, gradientes, brillo y ajuste de tonos |

## 🌠 ¿Cómo se genera la imagen?

1. **Cámara virtual:** se configura la posición de la cámara y se proyecta un rayo por muestra de cada píxel.
2. **Trazado de rayos:** se avanza en pequeños pasos por el espacio tridimensional.
3. **Lente gravitacional simplificada:** cerca del agujero negro se curva la trayectoria mediante una aproximación matemática inspirada en órbitas nulas de Schwarzschild. Esto produce distorsión y múltiples imágenes del disco.
4. **Horizonte de eventos:** si la trayectoria alcanza el radio asignado al horizonte, el rayo se considera capturado y no recibe fondo estelar.
5. **Disco de acreción:** las intersecciones con el plano del disco aportan luz según su radio, temperatura de color artística, turbulencias y una asimetría de brillo inspirada en el efecto Doppler.
6. **Fondo espacial:** los rayos que escapan muestran estrellas y nebulosas generadas matemáticamente (sin texturas externas).
7. **Imagen final:** se combinan las contribuciones, se aplica corrección de tonos y se guarda un archivo **PNG real**, generado directamente por el programa.

> **Importante:** el algoritmo representa curvatura gravitacional aproximada para fines gráficos. No resuelve íntegramente las ecuaciones de Einstein, no modela la rotación de Kerr, ni calcula espectros físicos u observaciones reales de TON 618. El halo añadido cerca de la esfera de fotones es un recurso visual.

## 🛠️ Requisitos

- Linux / Ubuntu (probado en un entorno Linux).
- **g++** compatible con C++17.
- **GNU Make**.
- Procesador (CPU). **No hace falta GPU dedicada ni instalar bibliotecas externas.**

Para instalar compilador y Make en Ubuntu:

```bash
sudo apt update
sudo apt install build-essential
```

## 🚀 Compilar y ejecutar

Desde la raíz del repositorio de la materia:

```bash
cd Imagen-Ray-Tracing/AgujeroNegro
make
make render
```

La imagen se guarda en `renders/AgujeroNegro.png`. Podés abrirla con el visor de imágenes de Ubuntu:

```bash
xdg-open renders/AgujeroNegro.png
```

### ⚡ Vista previa rápida

```bash
make preview
xdg-open renders/Preview.png
```

### 🔎 Cambiar resolución y calidad

```bash
./bin/AgujeroNegro --width 1920 --height 1080 --samples 4 --output renders/AgujeroNegro-HD.png
```

`--samples` indica la cantidad de muestras por píxel (1 a 16), y `--threads` permite elegir el número de hilos de CPU. El programa detecta automáticamente los hilos disponibles si no lo indicás.

```bash
./bin/AgujeroNegro --width 1280 --height 720 --samples 2 --threads 8 --output renders/Prueba.png
```

Usá `make clean` para eliminar el ejecutable. Los renders quedan conservados.

## 📁 Estructura

```text
Imagen-Ray-Tracing/
    ├── Makefile                # Compila y renderiza con GNU Make
    ├── README.md               # Explicación del trabajo
    ├── .gitignore              # Evita subir binarios
    ├── src/
    │   └── main.cpp           # Cámara, rayos, escena y PNG
    └── renders/
        └── AgujeroNegro.png   # Imagen original renderizada por CPU
```

## 🗣️ Implementación

> Implementé un ray tracer en C++17 que funciona exclusivamente con la CPU. Para cada píxel proyecto rayos desde una cámara virtual y los integro a través de una escena 3D. Cuando un rayo cruza el disco, calculo su contribución de luz; cuando llega al horizonte de eventos, lo considero capturado; y cuando escapa, evalúo un campo de estrellas. Para representar un agujero negro usé una aproximación de curvatura gravitacional, de modo que el disco y las estrellas aparezcan distorsionados. Finalmente convierto los colores calculados en un PNG. No uso tecnología RTX de NVIDIA ni tampoco el AMD Radeon Ray Tracing de AMD: aplico ray tracing por software.

---

📘 **Proyecto académico — Computación Gráfica y Ambientes Virtuales · CRUC-IUA · 2026**

🌌 *Inspirado en TON 618: arte computacional, no reproducción científica.*
