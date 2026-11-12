#ifndef CHECKER_H
#define CHECKER_H

#include <cmath>

#include <material.h>

/// Difuso con tablero de ajedrez en el plano xz: las casillas "impares" cambian
/// los coeficientes ambiental y difuso para poder alternar los colores del
/// tablero. Casillas impares -> ka2 y kd2 en lugar de ka y kd (heredados de la
/// clase base)
class Checker : public Diffuse {
public:
    glm::vec3 ka2 = glm::vec3(0.02f);
    glm::vec3 kd2 = glm::vec3(0.05f);
    float size = 1.0f;                  ///< lado de cada casilla

    glm::vec3 ka_at(const glm::vec3& p) const override { return odd(p) ? ka2 : ka; }
    glm::vec3 kd_at(const glm::vec3& p) const override { return odd(p) ? kd2 : kd; }

private:
    /// True si p cae en una casilla impar
    bool odd(const glm::vec3& p) const {
        // TODO (paso 6): textura tipo tablero.
        // Dividir x y z por el largo del lado de cada casilla y quedarse con la
        // parte entera con std::floor, no hacer un cast a int: el cast trunca
        // hacia cero y la fila de casillas que rodea al eje queda del doble
        // de ancho. La casilla es impar si la suma de los dos enteros es
        // impar.
        // Usar sólo las componentes x y z: al usar y también y si el objeto es
        // un plano horizontal en y = 0, la función floor(y) salta entre -1 y 0
        // por el redondeo y no sale el patrón de tablero.
        (void)p;
        return false;
    }
};

#endif // CHECKER_H
