
#include "ResourceManager.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ResourceManager::ResourceManager(
    const std::filesystem::path& assets_root
) : assets_root_(assets_root) {

    if (!std::filesystem::is_directory(assets_root_)) {
        throw std::runtime_error(
            "No existe la carpeta de recursos: " +
            assets_root_.string()
        );
    }
}

std::string ResourceManager::read_shader_file(
    const std::string& shader_file,
    const std::string& shader_type
) const {
    auto path = assets_root_ / shader_file;

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(
            "No se pudo abrir " + shader_type +
            ": " + path.string()
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad()) {
        throw std::runtime_error(
            "Error leyendo archivo: " + path.string()
        );
    }

    return buffer.str();
}

const ShaderSource& ResourceManager::load_shader_source(
    const std::string& key,
    const std::string& vs_file,
    const std::string& fs_file,
    const std::string& gs_file
) {
    if (key.empty() || vs_file.empty() || fs_file.empty()) {
        throw std::invalid_argument(
            "Clave, vertex y fragment son obligatorios"
        );
    }

    auto it = shaders_sources_.find(key);

    if (it != shaders_sources_.end()) {
        const ShaderEntry& entry = it->second;

        if (entry.vs_file != vs_file ||
            entry.fs_file != fs_file ||
            entry.gs_file != gs_file) {
            throw std::runtime_error(
                "La clave '" + key +
                "' ya existe con archivos diferentes"
            );
        }

        // Cache HIT: no leemos de disco.
        return entry.source;
    }

    // Cache MISS: leer archivos.
    ShaderEntry entry;

    entry.vs_file = vs_file;
    entry.fs_file = fs_file;
    entry.gs_file = gs_file;

    entry.source.vs = read_shader_file(
        vs_file, "vertex shader"
    );

    entry.source.fs = read_shader_file(
        fs_file, "fragment shader"
    );

    if (!gs_file.empty()) {
        entry.source.gs = read_shader_file(
            gs_file, "geometry shader"
        );
    }

    auto result = shaders_sources_.emplace(
        key, std::move(entry)
    );

    return result.first->second.source;
}

const ShaderSource& ResourceManager::get_shader_source(
    const std::string& key
) const {
    auto it = shaders_sources_.find(key);

    if (it == shaders_sources_.end()) {
        throw std::runtime_error(
            "No existe el recurso: " + key
        );
    }

    return it->second.source;
}

const ImageData& ResourceManager::load_image(
    const std::string& key, const std::string& filename, bool flip_vertically) {
    auto found=images_.find(key);
    if(found!=images_.end()) {
        if(found->second.file!=filename || found->second.flipped!=flip_vertically)
            throw std::runtime_error("La imagen '"+key+"' ya esta en cache con otras opciones");
        return found->second.data;
    }
    const auto path=assets_root_/filename;
    // Estado de stb_image: se fija para CADA carga y se restaura, cielo sin invertir.
    stbi_set_flip_vertically_on_load(flip_vertically ? 1 : 0);
    int w=0,h=0,n=0;
    unsigned char* raw=stbi_load(path.string().c_str(),&w,&h,&n,4);
    stbi_set_flip_vertically_on_load(0);
    if(!raw) throw std::runtime_error("Fallo leyendo imagen: "+path.string());
    ImageEntry entry;
    entry.file=filename; entry.flipped=flip_vertically;
    entry.data.width=w; entry.data.height=h; entry.data.channels=n;
    entry.data.pixels.assign(raw,raw + static_cast<std::size_t>(w)*h*4);
    stbi_image_free(raw);
    return images_.emplace(key,std::move(entry)).first->second.data;
}
const ImageData& ResourceManager::get_image(const std::string& key) const {
    auto it=images_.find(key);
    if(it==images_.end()) throw std::runtime_error("No existe imagen: "+key);
    return it->second.data;
}
const std::string& ResourceManager::load_text(const std::string& key, const std::string& file) {
    return texts_.emplace(key,read_shader_file(file,"archivo GLSL")).first->second;
}
void ResourceManager::clear() {
    shaders_sources_.clear(); images_.clear(); texts_.clear();
}
