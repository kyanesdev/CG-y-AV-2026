#pragma once

#include <dlfdm/defines.h>

// Ajuste de jugabilidad aplicado SOLO a la copia de parametros usada en AeroIUA.
// El FDM de la catedra y los controles originales NO se modifican.
namespace aeroiua {
    constexpr float kSpeedResponse = 2.0f;

    inline void configure_training_response(dlfdm::AircraftParameters& params) {
        // El trim original iguala aproximadamente empuje y arrastre a 150 m/s.
        // Escalar ambos por el mismo factor mantiene ese equilibrio inicial.
        // Fuera del trim, el empuje neto (T-D) aumenta en magnitud, por lo que
        // W (motor) acelera y S (motor al minimo) desacelera mas rapido.
        // NO es un modelo aeronautico calibrado; es el modo escuela del juego.
        params.engine.max_thrust_sl *= kSpeedResponse;
        params.CD0 *= kSpeedResponse;
        params.CDa *= kSpeedResponse;
    }
}
