# 🎮 Computación Gráfica y Ambientes Virtuales — 2026

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green?logo=opengl)
![GLSL](https://img.shields.io/badge/GLSL-4.60-orange)
![Ubuntu](https://img.shields.io/badge/Ubuntu-24.04_LTS-E95420?logo=ubuntu&logoColor=white)
![Status](https://img.shields.io/badge/Estado-En_desarrollo-yellow)

## 📚 Descripción del proyecto

Repositorio académico de la materia **Computación Gráfica y Ambientes Virtuales (CGyAV)**, correspondiente a la carrera de **Ingeniería Informática** del **Centro Regional Universitario Córdoba (CRUC)**, perteneciente al **Instituto Universitario Aeronáutico (IUA)**, Córdoba, Argentina.

El propósito de este repositorio es documentar y desarrollar los contenidos prácticos de la asignatura, aplicando los conceptos fundamentales de la computación gráfica mediante programación en **C++**, la API **OpenGL** y el lenguaje de sombreado **GLSL**.

El desarrollo se realiza siguiendo los contenidos, las consignas y las herramientas indicadas por la cátedra.

## 👨‍💻 Datos del alumno

| Campo | Información |
|---|---|
| **Alumno** | Kevin Leonel Yanes |
| **Carrera** | Ingeniería Informática |
| **Institución** | CRUC — IUA |
| **Ubicación** | Córdoba, Argentina 🇦🇷 |
| **Año académico** | 2026 |
| **GitHub** | [@kyanesdev](https://github.com/kyanesdev) |

## 🎯 Objetivos de la materia

Comprender y aplicar los fundamentos teóricos y prácticos de la computación gráfica y los ambientes virtuales.

Los objetivos de este repositorio son:

- 📐 Comprender la representación y el procesamiento de geometría.
- 🎨 Trabajar con el pipeline gráfico de OpenGL.
- 💻 Desarrollar aplicaciones gráficas en C++.
- 🖥️ Implementar shaders utilizando GLSL.
- 📦 Administrar recursos gráficos y archivos mediante módulos reutilizables.
- 🧮 Aplicar los conceptos matemáticos necesarios para la representación gráfica.
- ✈️ Integrar los conocimientos de la asignatura en un trabajo práctico final.

## 🛠️ Tecnologías y herramientas

| Tecnología | Utilización |
|---|---|
| **C++17** | Lenguaje principal de programación |
| **OpenGL 4.6** | API de computación gráfica |
| **GLSL 4.60** | Programación de shaders |
| **GLFW** | Creación de ventanas y gestión de eventos |
| **GLAD** | Carga de funciones de OpenGL |
| **GLM** | Operaciones matemáticas para gráficos |
| **GCC / G++** | Compilación de código C y C++ |
| **GNU Make** | Automatización de compilaciones |
| **Visual Studio Code** | Entorno de desarrollo |
| **Ubuntu 24.04 LTS** | Sistema operativo de desarrollo |
| **Git / GitHub** | Control de versiones |

## 📂 Organización del repositorio

El repositorio se divide en proyectos independientes, de acuerdo con las actividades prácticas de la materia.

```text
CGyAV/
├── README.md
├── .gitignore
└── Practico/
    ├── Proyecto-basico/
    │   └── ogl-project-template-revAgo26/
    │       └── ogl-project-template/
    │
    ├── Guia-1/
    │   ├── Parte-1-Triangulo/
    │   └── Parte-2-ResourceManager/
    │
    └── ...
```

La estructura se ampliará progresivamente conforme se desarrollen las siguientes guías de ejercicios.

## 📖 Guías de trabajos prácticos

| Guía | Contenido | Estado |
|---|---|---|
| **Guía 01** | Introducción a OpenGL y GLSL | ✅ Completada |
| **Guía 02** | Actividades de la segunda guía | ⏳ Pendiente |
| **Guía 03** | Actividades de la tercera guía | ⏳ Pendiente |
| **Guía 04** | Actividades de la cuarta guía | ⏳ Pendiente |
| **Guía 05** | Actividades de la quinta guía | ⏳ Pendiente |
| **Guía 06** | Actividades de la sexta guía | ⏳ Pendiente |

Las actividades se implementarán respetando las consignas oficiales de la asignatura.

## ✅ Guía 01 — Introducción a OpenGL y GLSL

### 🔺 Parte 1: Primer triángulo con OpenGL

Se desarrolló una aplicación gráfica utilizando el template oficial proporcionado por la cátedra.

**Funcionalidades implementadas:**

- Inicialización de GLFW.
- Creación de una ventana con contexto OpenGL 4.6 Core Profile.
- Inicialización del cargador de funciones GLAD.
- Definición de los vértices de un triángulo.
- Creación y configuración de VAO y VBO.
- Implementación de vertex shader y fragment shader mediante GLSL.
- Compilación, enlazado y utilización de un programa de shaders.
- Renderizado de un triángulo de color sólido.
- Detección y visualización de errores de compilación de shaders.
- Liberación de los recursos gráficos utilizados.

**Resultado:** aplicación OpenGL que representa un triángulo sobre un fondo de color uniforme.

### 📦 Parte 2: ResourceManager

Se implementó un módulo independiente utilizando exclusivamente **C++17 y su biblioteca estándar**, sin dependencias de OpenGL.

**Funcionalidades implementadas:**

- Lectura del código fuente de shaders desde archivos.
- Almacenamiento de los recursos en memoria mediante una caché.
- Identificación de los recursos a través de claves lógicas.
- Reutilización de recursos previamente cargados.
- Prevención de lecturas repetidas del mismo recurso.
- Detección de conflictos entre claves y archivos.
- Manejo de errores mediante excepciones.
- Recuperación de recursos mediante referencias constantes.
- Limpieza de la caché mediante `clear()`.

La implementación utiliza `std::unordered_map`, `std::ifstream`, `std::filesystem` y otras herramientas de la biblioteca estándar.

**Resultado:** módulo compilado y probado mediante un programa de consola independiente.

## 🚀 Compilación y ejecución

### Requisitos

Para Ubuntu 20.04 o superior, las dependencias indicadas por la cátedra pueden instalarse mediante:

```bash
sudo apt update

sudo apt install -y \
  build-essential \
  mesa-common-dev \
  libgl-dev \
  libglu1-mesa-dev \
  libglm-dev \
  libglfw3-dev \
  mesa-utils
```

Se requiere un controlador gráfico compatible con OpenGL 4.5 o superior.

### 🔺 Ejecutar la Parte 1

Desde la raíz del repositorio:

```bash
cd Practico/Guia-1/Parte-1-Triangulo

make build

./bin/ogl-app
```

### 📦 Ejecutar la Parte 2

```bash
cd Practico/Guia-1/Parte-2-ResourceManager

g++ -std=c++17 -Wall -Wextra \
  ResourceManager.cpp main.cpp -o test

./test
```

## ✈️ Trabajo práctico integrador

Como proyecto integrador se contempla el desarrollo de una **simulación gráfica de vuelo de un avión mediante OpenGL**.

El propósito será integrar los conocimientos adquiridos durante la materia y representar gráficamente los elementos necesarios para una experiencia de simulación de vuelo.

Su alcance, funcionalidades y arquitectura se definirán de acuerdo con las consignas de la cátedra y los contenidos efectivamente estudiados.

El desarrollo priorizará la comprensión de los conceptos y la capacidad de explicar cada componente implementado.

## 🧠 Metodología de aprendizaje

El desarrollo de cada trabajo práctico sigue una secuencia de aprendizaje:

1. Analizar la consigna oficial de la guía.
2. Identificar los conceptos teóricos necesarios.
3. Diseñar una solución acorde con los contenidos de la materia.
4. Implementar el código en C++ y, cuando corresponda, OpenGL/GLSL.
5. Compilar, ejecutar y verificar los resultados.
6. Comprender y documentar las decisiones de implementación.

El objetivo no es solamente obtener un programa funcional, sino también **entender cómo funciona y poder justificar técnicamente su implementación**.

## 📌 Estado del proyecto

🚧 **En desarrollo — Ciclo académico 2026**

- ✅ Preparación del entorno de desarrollo.
- ✅ Configuración de OpenGL, GLFW, GLAD y GLM.
- ✅ Compilación y ejecución del template oficial.
- ✅ Resolución de la Guía Práctica 01.
- ⏳ Desarrollo de las guías prácticas restantes.
- ⏳ Trabajo práctico integrador.

---

## 🎓 Información académica

Este repositorio tiene fines exclusivamente académicos y educativos.

Los ejercicios se desarrollan a partir del material proporcionado por la cátedra de Computación Gráfica y Ambientes Virtuales. Los componentes de terceros y los templates suministrados conservan sus respectivas condiciones de uso y atribuciones.

**Desarrollado por Kevin Leonel Yanes** 🇦🇷

Ingeniería Informática — CRUC IUA — 2026

🔗 [GitHub — kyanesdev](https://github.com/kyanesdev)
