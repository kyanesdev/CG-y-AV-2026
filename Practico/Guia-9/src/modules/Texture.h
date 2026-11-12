#pragma once
#include <array>
#include "ResourceManager.h"

// Duenio unico del objeto OpenGL; RAII, sin copias y con movimiento.
class Texture {
public:
    Texture() = default;
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    void load_2d(const ImageData& image);
    void load_cubemap(const std::array<const ImageData*, 6>& faces);
    void bind(unsigned int unit) const;
    void clear();
    unsigned int id() const { return id_; }
private:
    unsigned int id_ = 0U;
};
