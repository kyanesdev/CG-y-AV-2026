#pragma once
#include "Mesh.h"
#include <cstdint>

// Terreno de N x N celdas. Las coordenadas x/z quedan centradas en el origen.
struct TerrainConfig {
    unsigned cells = 400;
    float spacing = 25.0f;          // metros entre vertices
    float amplitude = 150.0f;      // amplitud nominal del relieve en metros
    float frequency = 0.010f;      // frecuencia en nodos de grilla
    std::uint32_t seed = 2026U;    // semilla reproducible
};

// Fuente de alturas independiente de la geometria (Perlin fBm, cinco octavas).
class Alturas {
public:
    explicit Alturas(const TerrainConfig& config) : cfg_(config) {}
    float operator()(int i, int j) const;
private:
    TerrainConfig cfg_;
};

MeshData make_terrain(const TerrainConfig& cfg, float& minHeight, float& maxHeight);
