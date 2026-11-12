
#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

struct ShaderSource {
    std::string vs;
    std::string fs;
    std::string gs;
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

    void clear();

private:
    // Conservamos los nombres para detectar conflictos.
    struct ShaderEntry {
        ShaderSource source;
        std::string vs_file;
        std::string fs_file;
        std::string gs_file;
    };

    std::filesystem::path assets_root_;

    std::unordered_map<std::string, ShaderEntry>
        shaders_sources_;

    std::string read_shader_file(
        const std::string& shader_file,
        const std::string& shader_type
    ) const;
};
