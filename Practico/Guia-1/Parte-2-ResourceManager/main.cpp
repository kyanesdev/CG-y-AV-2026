
#include "ResourceManager.h"

#include <iostream>
#include <exception>

int main() {
    try {
        ResourceManager manager("assets/shaders");

        std::cout << "1. Primera carga\n";

        const ShaderSource& first =
            manager.load_shader_source(
                "basic", "basic.vs", "basic.fs"
            );

        std::cout << "Vertex shader: "
                  << first.vs.size()
                  << " bytes\n";

        std::cout << "\n2. Segunda carga\n";

        const ShaderSource& second =
            manager.load_shader_source(
                "basic", "basic.vs", "basic.fs"
            );

        std::cout << "Mismo objeto en memoria: "
                  << std::boolalpha
                  << (&first == &second)
                  << '\n';

        std::cout << "\n3. Clave inexistente\n";

        try {
            manager.get_shader_source("inexistente");
        }
        catch (const std::exception& e) {
            std::cout << "Error detectado: "
                      << e.what() << '\n';
        }

        std::cout << "\n4. Conflicto de clave\n";

        try {
            manager.load_shader_source(
                "basic", "otro.vs", "otro.fs"
            );
        }
        catch (const std::exception& e) {
            std::cout << "Error detectado: "
                      << e.what() << '\n';
        }

        std::cout << "\n5. Limpiar cache\n";
        manager.clear();

        try {
            manager.get_shader_source("basic");
        }
        catch (const std::exception& e) {
            std::cout << "Error detectado: "
                      << e.what() << '\n';
        }

        std::cout << "\nPruebas finalizadas\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error general: "
                  << e.what() << '\n';
        return 1;
    }

    return 0;
}
