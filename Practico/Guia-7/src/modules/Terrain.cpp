#include "Terrain.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
// Implementacion autocontenida de ruido Perlin 2D: no cambia el concepto del
// enunciado (ruido procedural + fBm), pero evita depender de descargas externas.
std::uint32_t hash2(int x, int y, std::uint32_t seed) {
    std::uint32_t h = seed ^ (static_cast<std::uint32_t>(x) * 0x9e3779b9u)
                           ^ (static_cast<std::uint32_t>(y) * 0x85ebca6bu);
    h ^= h >> 16; h *= 0x7feb352du; h ^= h >> 15; h *= 0x846ca68bu; h ^= h >> 16;
    return h;
}
float fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }
float lerp(float a, float b, float t) { return a + t * (b - a); }
float gradient(int ix, int iy, float x, float y, std::uint32_t seed) {
    const unsigned dir = hash2(ix, iy, seed) & 7u;
    const float dx = x - ix, dy = y - iy;
    constexpr float q = 0.70710678f;
    switch (dir) {
        case 0: return dx;
        case 1: return -dx;
        case 2: return dy;
        case 3: return -dy;
        case 4: return q * ( dx + dy);
        case 5: return q * (-dx + dy);
        case 6: return q * ( dx - dy);
        default:return q * (-dx - dy);
    }
}
float perlin(float x, float y, std::uint32_t seed) {
    const int ix = static_cast<int>(std::floor(x));
    const int iy = static_cast<int>(std::floor(y));
    const float u = fade(x - ix), v = fade(y - iy);
    const float p = lerp(gradient(ix,   iy,   x,y,seed),
                         gradient(ix+1, iy,   x,y,seed),u);
    const float q = lerp(gradient(ix,   iy+1, x,y,seed),
                         gradient(ix+1, iy+1, x,y,seed),u);
    return lerp(p,q,v);
}
}

float Alturas::operator()(int i, int j) const {
    float value = 0.0f, weight = 1.0f, total = 0.0f, freq = cfg_.frequency;
    for (unsigned octave = 0; octave < 5; ++octave) {
        value += weight * perlin(j * freq, i * freq, cfg_.seed + octave * 19U);
        total += weight;
        weight *= 0.5f;
        freq *= 2.0f;
    }
    return cfg_.amplitude * value / total;
}

MeshData make_terrain(const TerrainConfig& cfg, float& minHeight, float& maxHeight) {
    if (cfg.cells == 0 || cfg.spacing <= 0 || cfg.frequency <= 0 || cfg.amplitude < 0)
        throw std::invalid_argument("Configuracion de terreno invalida");
    Alturas height(cfg);
    const unsigned N = cfg.cells, row = N + 1;
    const float half = N * cfg.spacing * 0.5f;
    MeshData data;
    data.vertices.reserve(static_cast<std::size_t>(row) * row);
    data.indices.reserve(static_cast<std::size_t>(N) * N * 6);
    minHeight = 1e30f; maxHeight = -1e30f;

    // Hay N+1 vertices por lado (i,j in [0,N]).
    for (unsigned i = 0; i <= N; ++i) {
        for (unsigned j = 0; j <= N; ++j) {
            float h = height(static_cast<int>(i), static_cast<int>(j));
            minHeight = std::min(minHeight, h); maxHeight = std::max(maxHeight, h);
            auto sample = [&height](int row, int col){ return height(row,col); };
            // Diferencias centrales, incluso en el borde: el ruido esta definido fuera de la malla.
            const float dx = sample(static_cast<int>(i), static_cast<int>(j)-1)
                           - sample(static_cast<int>(i), static_cast<int>(j)+1);
            const float dz = sample(static_cast<int>(i)-1, static_cast<int>(j))
                           - sample(static_cast<int>(i)+1, static_cast<int>(j));
            glm::vec3 normal = glm::normalize(glm::vec3(dx, 2.0f*cfg.spacing, dz));
            data.vertices.push_back({
                {j * cfg.spacing - half, h, i * cfg.spacing - half},
                normal,
                {float(j)/N, float(i)/N}
            });
        }
    }

    // Cada celda: dos triangulos CCW desde arriba. En total 2*N*N.
    auto index = [row](unsigned i, unsigned j) { return i * row + j; };
    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            const unsigned a=index(i,j), b=index(i,j+1);
            const unsigned d=index(i+1,j), c=index(i+1,j+1);
            data.indices.insert(data.indices.end(), {a,d,c, a,c,b});
        }
    }
    return data;
}
