
#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

struct ShaderSource {
    std::string vs;
    std::string fs;
    std::string gs;
};

struct ImageData {
    int width = 0, height = 0, channels = 0;  // canales originales
    std::vector<unsigned char> pixels;        // siempre RGBA
};

class ResourceManager {
public:
    explicit ResourceManager(
        const std::filesystem::path& assets_root
    );

    const ShaderSource& load_shader_source(
        const std::string& key,
        const std::string& vs_file,
        const std::string& fs_file,
        const std::string& gs_file = ""
    );

    // La referencia es valida hasta clear()
    // o la destruccion del ResourceManager.
    const ShaderSource& get_shader_source(
        const std::string& key
    ) const;

    const ImageData& load_image(const std::string& key, const std::string& file, bool flip_vertically);
    const ImageData& get_image(const std::string& key) const;
    // Codigo GLSL comun en Guia 09; aun no se usa en Guia 08.
    const std::string& load_text(const std::string& key, const std::string& file);
    void clear();

private:
    // Conservamos los nombres para detectar conflictos.
    struct ShaderEntry {
        ShaderSource source;
        std::string vs_file;
        std::string fs_file;
        std::string gs_file;
    };

    struct ImageEntry { ImageData data; std::string file; bool flipped; };
    std::unordered_map<std::string, ImageEntry> images_;
    std::unordered_map<std::string, std::string> texts_;
    std::filesystem::path assets_root_;

    std::unordered_map<std::string, ShaderEntry>
        shaders_sources_;

    std::string read_shader_file(
        const std::string& shader_file,
        const std::string& shader_type
    ) const;
};
