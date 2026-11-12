# Guía 10: 8 escenas resueltas, preguntas orientativas y justificaciones

**Fuente:** consigna oficial de la guía práctica 10 (06/11/2026), especialmente su sección final de «Respuestas orientativas». Estas explicaciones se ofrecen para **estudio y práctica**, no para consultar modelos de IA durante el parcial.

## Escena 1 — Tres luces y sombras de colores

**Código:** `src/escenas/e01-sombras-color.cpp`. **Modelo:** una esfera blanca delante de tres luces puntuales de colores primarios (rojo, verde, azul) y piso blanco.

**Qué tiene que verse:** sombras de color amarillo, magenta y cian. Fuera de las sombras, se combinan las tres iluminaciones. Debajo de la esfera, si ninguna luz llega, sólo aporta la iluminación ambiental.

**Pregunta:** ¿De qué color queda la región donde se bloquean a la vez la luz roja y la verde?

**Respuesta:** Azul. El rayo de sombra hacia la luz roja está bloqueado, igual que el dirigido a la luz verde, pero el dirigido a la luz azul llega. El azul es la única iluminación directa sobre esa zona, más el término ambiental.

**Interpretación extra:** si un programador usara `break` en lugar de `continue` al encontrar una luz bloqueada, dejaría de evaluar también las demás luces. El color de las sombras sería incorrecto. **La sombra se evalúa por cada luz, no por toda la escena de una sola vez.**

## Escena 2 — Dos espejos paralelos

**Código:** `src/escenas/e02-espejos-paralelos.cpp`. **Parámetro fundamental:** `mirror->kr = 0.8f`.

**Qué tiene que verse:** repetición aparentemente infinita de una esfera naranja, pero con número finito de copias y brillo que se pierde con los rebotes. Las paredes son planos infinitos.

**Pregunta 1:** ¿En qué rebote cae por debajo de 1 %? ¿Cuánto aporta el último rebote de profundidad 8?

**Respuesta:** La fracción que queda tras `n` rebotes es `0.8^n`. Se busca `0.8^n < 0.01`; usando logaritmos, `n > log(0.01)/log(0.8) ≈ 20.64`, por lo que desde el **rebote 21** cae por debajo de 1 %. A profundidad 8, `0.8^8 ≈ 0.1678`: aún conserva aproximadamente **16.8 %**. Por eso el corte por profundidad produce un final perceptible.

**Pregunta 2:** ¿Por qué usar un umbral de peso acumulado en vez de siempre cortar a profundidad fija?

**Respuesta:** Porque las ramas que ya aportan muy poco se podrían cortar temprano, ahorrando cómputo, y las ramas de alta reflectancia podrían seguir más lejos para que desaparezca el corte brusco.

**Posible prueba:** cambiar `scene.set_max_depth(8U)` por `2U`: se ven menos copias.

## Escena 3 — Pasillo finito de espejos

**Código:** `src/escenas/e03-pasillo.cpp`. **Conceptos:** paredes rectangulares formadas por 2 triángulos cada una; esfera espejo con `kr = 0.85` y `ks = 1`.

**Pregunta 1:** ¿Por qué con `max_depth=1` una pared se ve casi negra donde refleja la otra?

**Respuesta:** Un rayo primario toca el primer espejo (`depth=0`) y produce un rayo reflejado (`depth=1`) que toca el segundo. El siguiente rebote alcanzaría `depth=2` y se trunca, de modo que sobre el segundo espejo queda solamente su pequeña componente local `kd=0.02`.

**Pregunta 2:** ¿Por qué la esfera espejo necesita `ks` distinto de cero para mostrar el brillo directo de la luz?

**Respuesta:** La luz puntual no es una esfera geométrica y un rayo reflejado no la intercepta. El destello blanco que representa el foco lo calcula Phong mediante `ks` y `shininess`; el reflejo del entorno lo calcula `kr`.

**Pregunta 3:** ¿Cambiar el orden de los vértices invierte la imagen de las paredes?

**Respuesta:** En este trazador no, salvo diferencias mínimas de redondeo: `Hit::set_face` orienta la normal hacia el rayo incidente y un espejo refleja desde cualquiera de los dos lados. En otros programas con descarte de caras o normales sin reorientar sí podría importar.

## Escena 4 — Vidrio y agua (sombra y refracción)

**Código:** `src/escenas/e04-vidrio-agua.cpp`. **Materiales:** `Dielectric` de índice 1.5 (vidrio) y 1.33 (agua).

**Qué tiene que verse:** objetos y tablero distorsionados o invertidos a través de las esferas; sombras más claras que las de cuerpos opacos.

**Pregunta:** ¿Qué fracción de luz pasa por cada esfera cuando el rayo de sombra la atraviesa?

**Respuesta:** Se cruzan dos superficies. Vidrio: `0.8 * 0.8 = 0.64` (**64 %**). Agua: `0.9 * 0.9 = 0.81` (**81 %**). La sombra es más clara en agua bajo este parámetro porque deja pasar mayor fracción del rayo de sombra.

**Detalle importante:** `transparency` es fracción que deja pasar cada superficie **al rayo de sombra**. Para los rayos que construyen el aspecto de la esfera, la dirección refractada se calcula con Snell y el balance reflejado/refractado con Schlick (Fresnel).

## Escena 5 — Índices de refracción

**Código:** `src/escenas/e05-indices.cpp`. **Materiales:** aire 1.0, agua 1.33, vidrio 1.5, diamante 2.42, en esferas geométricamente iguales.

**Qué tiene que verse:** con `ior = 1` el fondo casi no se deforma; con índices mayores aumenta la desviación. El diamante refleja relativamente más.

**Pregunta:** ¿Por qué aparece un borde tenue reflejado en la esfera de índice 1 si la física dice que no debería reflejar?

**Respuesta:** La aproximación de Schlick usa `F=F0+(1-F0)(1-cos(theta))^5`. Si `ior=1` entonces `F0=0`, pero queda `(1-cos(theta))^5`, diferente de cero a ángulos oblicuos. Eso produce una reflexión espuria cerca del borde: **es una limitación de Schlick**, no un fenómeno físico real para dos medios con índice igual.

## Escena 6 — Esfera maciza, cáscara y burbuja

**Código:** `src/escenas/e06-hueca-burbuja.cpp`. **Materiales:** vidrio macizo `ior=1.5`, vidrio hueco con una esfera interior `ior=1/1.5`, burbuja de aire en agua `ior=1/1.33`.

**Qué tiene que verse:** el tablero aparece invertido a través del vidrio macizo; en la cáscara se ve con poca distorsión en el centro; a través de la burbuja, derecho y más pequeño. El anillo de la burbuja es muy reflectante por Fresnel y la reflexión interna total.

**Pregunta:** ¿Una burbuja de aire rodeada de agua converge o diverge los rayos?

**Respuesta:** Es **divergente**, porque el índice relativo de aire respecto al agua es menor que 1. Por la geometría curva y el salto de índice, los rayos se desvían de manera equivalente a una lente divergente; la imagen del tablero se ve derecha y reducida.

**Posible error:** si el programa no atiende la reflexión interna total cuando un rayo intenta entrar a un medio de índice menor, puede generar una dirección refractada indefinida (`NaN`) y zonas negras falsas.

## Escena 7 — Piscina y reflectancia de Fresnel

**Código:** `src/escenas/e07-piscina.cpp`. **Geometría:** superficie de agua en `y=0`, tablero de baldosas en `y=-1` y esferas sobre el agua.

**Pregunta 1:** ¿Por qué el agua refleja más a lo lejos?

**Respuesta:** Porque los rayos de cámara que llegan a zonas lejanas del plano inciden **más rasantes**: el ángulo respecto de la normal se aproxima a 90°, y Fresnel/Schlick predicen una reflectancia que tiende a 1. Cerca de la cámara el ángulo es menor, se ve más del fondo y menos del reflejo. En la guía la reflectancia cerca ronda 6 %; en zonas rasantes puede rondar 50 % o más.

**Pregunta 2:** ¿Por qué no hay cáusticas y por qué las sombras no se desvían al entrar al agua?

**Respuesta:** El trazador de Whitted calcula la iluminación en superficies difusas enviando **rayos de sombra rectos** hacia las luces. Estos rayos sólo se atenúan al atravesar dieléctricos; no se refractan ni concentran energía en un fondo. No hay rutas de luz refractada capaces de formar cáusticas ni de desplazar la sombra.

## Escena 8 — Matrioshka y recursión

**Código:** `src/escenas/e08-matrioshka.cpp`. **Geometría:** tres cáscaras transparentes concéntricas (dos esferas por cáscara) y una esfera roja central.

**Pregunta 1:** ¿Cuántos rayos por píxel puede trazar un árbol donde cada intersección bifurca en reflexión y refracción, hasta profundidad `n`?

**Respuesta:** En el peor caso el árbol binario completo tiene `1+2+4+...+2^n = 2^(n+1)-1` rayos. Con `n=12` son **8191 rayos primarios y secundarios por muestra**; con `-ss 3` son 9 muestras y el límite teórico es **73.719 rayos por píxel**, sin contar sombras. En una escena real muchas ramas terminan antes y el total efectivo suele ser menor.

**Pregunta 2:** ¿Por qué aumentar de 12 a 16 casi no cambia la imagen y sí el tiempo?

**Respuesta:** Cada camino atraviesa multiplicaciones por factores menores a 1 (reflectancia o transmitancia) y muchos caminos ya aportan color despreciable; sin embargo, el número posible de ramas aumenta exponencialmente. La mejora visual se vuelve mínima mientras el costo crece mucho.

**Qué probar:** editar `set_max_depth(8U)` en el archivo de escena 8 y comparar 3, 6, 8 y 12 **si hay tiempo**. Hacerlo primero a `-r 640 360 -ss 1`, no a calidad máxima.

---

## Cómo explicar correctamente una imagen desconocida

Al recibir una escena del profesor, observá y respondé siempre en este orden:

1. **Composición**: cuál es el piso; cantidad y tipo de objetos; materiales; luces; posición de la cámara.
2. **Evidencia visual**: colores de sombras, bordes brillantes, objetos duplicados, deformación del tablero, zonas negras, cambios hacia el horizonte.
3. **Causa en el código**: `kd` (color/Phong), `ks` (destello), `kr` (reflejo), `ior` (refracción/Fresnel), `transparency` (rayos de sombra), `max_depth` (límite).
4. **Predicción**: qué sucede si cambio **un** parámetro y mantengo los demás.
5. **Limitación si corresponde**: modelo aproximado de Schlick, sombras rectas sin cáusticas, profundidad finita.

**Ejemplo breve:** «Se observa una esfera que refleja el tablero y las otras esferas, porque tiene material `Mirror` y `kr=0.85`. Cada rebote multiplica la intensidad por 0.85, por lo que reflejos repetidos se oscurecen. El destello blanco procede de `ks` y `shininess`, no de que el rayo reflejado alcance la luz puntual».
