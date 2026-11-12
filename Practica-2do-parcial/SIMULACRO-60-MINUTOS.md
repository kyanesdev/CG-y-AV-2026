# Simulacro de parcial práctico — Guía 10

**Objetivo:** completar una escena con el código oficial sin IA ni Internet, generar el PNG y explicar qué sucede. **No es una consigna real del parcial**: es un ejercicio de entrenamiento diseñado a partir de las primitivas y materiales de la guía 10.

## Enunciado

Usá la escena 12 de práctica (`src/escenas/e12-plantilla-parcial.cpp`) y adaptala a estos datos:

- **Cámara:** ojo `(0, 3, 8)`, mira a `(0, 0.8, 0)`, arriba `(0, 1, 0)`, FOV 45°.
- **Fondo:** azul oscuro uniforme `(0.04, 0.06, 0.12)`.
- **Piso:** plano por `(0, 0, 0)`, normal `(0, 1, 0)`, material `Checker` con `size=0.7`, `kd=(0.8, 0.8, 0.8)`, `kd2=(0.1, 0.1, 0.1)`, `ka=(0.05, 0.05, 0.05)`, `ka2=(0.01, 0.01, 0.01)`.
- **Esfera izquierda:** centro `(-1.3, 0.9, 0)`, radio `0.9`, `Diffuse` rojo con `ka=(0.05, 0.01, 0.01)`, `kd=(0.8, 0.1, 0.1)`, `ks=(0.4, 0.4, 0.4)`, `shininess=40`.
- **Esfera derecha:** centro `(1.3, 0.9, 0)`, radio `0.9`, `Mirror` con `ka=(0.01, 0.01, 0.01)`, `kd=(0.08, 0.08, 0.08)`, `ks=(1,1,1)`, `shininess=100`, `kr=0.85`.
- **Luces:** dos luces puntuales: roja en `(-3, 6, 3)` con color `(3, 0, 0)` y azul en `(3, 6, 3)` con color `(0, 0, 3)`. Ambas con atenuación `(1, 0.12, 0.02)`.
- **Recursión:** profundidad máxima 8.
- **Salida:** PNG `mi-parcial.png` con 1280 × 720 y supermuestreo 3 × 3.

## Reglas de entrenamiento

Primero hacé una copia de la plantilla y editá sólo la escena 12 (por su registro existente no necesitás cambiar `escenas.cpp`). No cambies ninguna clase en `src/rt/`. Generá primero la vista de prueba `-r 640 360 -ss 1`. Inspeccioná que ambas esferas estén presentes, que el piso muestre el tablero, que las dos luces produzcan sombras coloreadas y que la esfera espejo refleje parte de la escena. Después generá el render final solicitado.

## Preguntas para responder por escrito (sin consultar soluciones al principio)

1. ¿Por qué hay zonas del piso que se ven solamente rojas o solamente azules?
2. ¿Cuál es la diferencia entre el brillo de la esfera espejo y el reflejo de la esfera roja sobre ella?
3. Si `kr` baja de 0.85 a 0.2, ¿qué debería cambiar de la esfera espejo y qué propiedad quedaría casi sin cambios?
4. Si se quita la luz azul, ¿qué ocurre con las sombras y con los términos de iluminación directa?
5. Si se baja `max_depth` a 0, ¿qué parte de la imagen de la esfera espejo puede seguir viéndose y qué desaparece?

## Autocorrección y revisión final

Marcá si lograste: (1) compilar sin errores; (2) renderizar en calidad de prueba; (3) identificar los 6 parámetros de la escena; (4) explicar los efectos sin inventar lo que no hace el motor; (5) generar `mi-parcial.png`; (6) entregar respuestas escritas con causa y consecuencia. Volvé a `GUIA-RAPIDA-PARCIAL.md` e `INTERPRETACION-ESCENAS.md` para revisar tus argumentos **después** de resolver el simulacro.
