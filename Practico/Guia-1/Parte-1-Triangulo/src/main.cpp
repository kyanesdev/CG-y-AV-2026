
#include <cstdlib>
#include <iostream>
#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// Configuracion de la ventana
static const char* kWindowTitle = "CGyAV - Guia 1 - Triangulo";
static constexpr int kWindowWidth = 800;
static constexpr int kWindowHeight = 600;
static constexpr int kGLVerMajor = 4;
static constexpr int kGLVerMinor = 6;

// Manejo de errores GLFW
static int glfw_error_code{};
static std::string glfw_error_str{};

// Funciones originales del template
static void error_callback(int error, const char* description);
static void framebuffer_size_callback(
    GLFWwindow* window, int width, int height);
static void processInput(GLFWwindow* window);
static void print_gl_version(void);

// Funciones para trabajar con shaders
static GLuint compileShader(
    GLenum type, const char* source);

static GLuint createShaderProgram();

// Vertex Shader (GLSL)
static const char* vertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader (GLSL)
static const char* fragmentShaderSource = R"(
#version 460 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}
)";

// Compila un shader y muestra errores si existen
static GLuint compileShader(
    GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength > 0 ? logLength : 1, '\0');
        glGetShaderInfoLog(
            shader,
            static_cast<GLsizei>(log.size()),
            nullptr,
            log.data()
        );

        std::cerr << "Error compilando shader:\n"
                  << log << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Crea el programa grafico con ambos shaders
static GLuint createShaderProgram()
{
    GLuint vertexShader = compileShader(
        GL_VERTEX_SHADER, vertexShaderSource);

    if (vertexShader == 0) {
        return 0;
    }

    GLuint fragmentShader = compileShader(
        GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success != GL_TRUE) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength > 0 ? logLength : 1, '\0');
        glGetProgramInfoLog(
            program,
            static_cast<GLsizei>(log.size()),
            nullptr,
            log.data()
        );

        std::cerr << "Error enlazando programa:\n"
                  << log << std::endl;

        glDeleteProgram(program);
        program = 0;
    }

    // Una vez enlazados, los shaders individuales
    // ya no son necesarios.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main()
{
    // Inicializacion original de GLFW
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed! "
                  << glfw_error_code << ": "
                  << glfw_error_str << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR, kGLVerMajor);
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR, kGLVerMinor);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        kWindowWidth,
        kWindowHeight,
        kWindowTitle,
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        std::cerr << "GLFW window creation failed! "
                  << glfw_error_code << ": "
                  << glfw_error_str << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!"
                  << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    print_gl_version();

    glfwSetFramebufferSizeCallback(
        window, framebuffer_size_callback);

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(
        window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSwapInterval(1);

    // --------------------------------------------------
    // PARTE 1: Definir vertices del triangulo
    // --------------------------------------------------

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Izquierda inferior
         0.5f, -0.5f, 0.0f, // Derecha inferior
         0.0f,  0.5f, 0.0f  // Superior
    };

    // --------------------------------------------------
    // PARTE 2: Crear shaders
    // --------------------------------------------------

    GLuint shaderProgram = createShaderProgram();

    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // --------------------------------------------------
    // PARTE 3: Configurar VAO y VBO
    // --------------------------------------------------

    GLuint VAO = 0;
    GLuint VBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Cada vertice tiene 3 valores float: x, y, z
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Desvincular configuracion
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --------------------------------------------------
    // PARTE 4: Bucle principal de renderizado
    // --------------------------------------------------

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Limpiar pantalla con el color del template
        glClearColor(
            51.0f / 256.0f,
            55.0f / 256.0f,
            76.0f / 256.0f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        // Activar programa de shaders
        glUseProgram(shaderProgram);

        // Seleccionar geometria
        glBindVertexArray(VAO);

        // Dibujar 3 vertices como un triangulo
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --------------------------------------------------
    // PARTE 5: Liberacion de recursos
    // --------------------------------------------------

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

// Callbacks originales del template

void error_callback(int error, const char* description)
{
    glfw_error_code = error;
    glfw_error_str = std::string(description);
}

void framebuffer_size_callback(
    [[maybe_unused]] GLFWwindow* window,
    int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)
        == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void print_gl_version(void)
{
    std::cout << " OpenGL Vendor: "
              << glGetString(GL_VENDOR) << std::endl;

    std::cout << " OpenGL Renderer: "
              << glGetString(GL_RENDERER) << std::endl;

    std::cout << " OpenGL Version: "
              << glGetString(GL_VERSION) << std::endl;

    std::cout << " GLSL Version: "
              << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;
}
