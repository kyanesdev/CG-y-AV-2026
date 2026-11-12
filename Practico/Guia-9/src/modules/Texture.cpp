#include "Texture.h"
#include <glad/gl.h>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cmath>

Texture::~Texture() { clear(); }
Texture::Texture(Texture&& other) noexcept { *this = std::move(other); }
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) { clear(); id_ = std::exchange(other.id_,0U); }
    return *this;
}
void Texture::clear() {
    if (id_) glDeleteTextures(1,&id_);
    id_ = 0U;
}
void Texture::bind(unsigned int unit) const { glBindTextureUnit(unit,id_); }
void Texture::load_2d(const ImageData& image) {
    if (image.width <= 0 || image.height <= 0 ||
        image.pixels.size() != std::size_t(image.width)*image.height*4)
        throw std::invalid_argument("Datos RGBA de textura 2D invalidos");
    clear();
    const int levels=1+static_cast<int>(std::floor(std::log2(std::max(image.width,image.height))));
    glCreateTextures(GL_TEXTURE_2D,1,&id_);                              // crear
    glTextureStorage2D(id_,levels,GL_RGBA8,image.width,image.height);    // reservar
    glTextureSubImage2D(id_,0,0,0,image.width,image.height,GL_RGBA,GL_UNSIGNED_BYTE,image.pixels.data()); // llenar
    glGenerateTextureMipmap(id_);                                       // generar niveles
    glTextureParameteri(id_,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(id_,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTextureParameteri(id_,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTextureParameteri(id_,GL_TEXTURE_WRAP_T,GL_REPEAT);
}
void Texture::load_cubemap(const std::array<const ImageData*,6>& faces) {
    if (!faces[0] || faces[0]->width <= 0 || faces[0]->height <= 0)
        throw std::invalid_argument("Primera cara del cubemap invalida");
    const int width=faces[0]->width, height=faces[0]->height;
    for(const auto* face:faces)
        if (!face || face->width!=width || face->height!=height ||
            face->pixels.size()!=std::size_t(width)*height*4)
            throw std::invalid_argument("Las seis caras deben tener el mismo formato RGBA y dimensiones");
    clear();
    glCreateTextures(GL_TEXTURE_CUBE_MAP,1,&id_);
    glTextureStorage2D(id_,1,GL_RGB8,width,height);
    for(unsigned face=0;face<6;++face)
        glTextureSubImage3D(id_,0,0,0,static_cast<GLint>(face),width,height,1,
                            GL_RGBA,GL_UNSIGNED_BYTE,faces[face]->pixels.data());
    glTextureParameteri(id_,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTextureParameteri(id_,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTextureParameteri(id_,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTextureParameteri(id_,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTextureParameteri(id_,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
}
