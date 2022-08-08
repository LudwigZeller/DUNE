//
// Created by Ludwig Zeller on 03.08.2022.
//
#include "utils.hpp"

/**
 * Syncs the thread with the defined FPS
 */
void while_timer() {
    // I am sorry for the static variable
    // But like this the main loop is readable -
    // and I can use the "chrono" namespace in this scope
    using namespace std::chrono;
    static auto timestamp = steady_clock::now();

    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - timestamp);
    if (elapsed < milliseconds(1000 / FPS)) {
//        std::cout << "Sleeping for " << (milliseconds(1000 / FPS) - elapsed).count() << "ms" << std::endl;
        std::this_thread::sleep_for(milliseconds(1000 / FPS) - elapsed);
    } else {
        std::cerr << "The thread is running " << (elapsed - milliseconds(1000 / FPS)).count() << "ms behind"
                  << std::endl;
    }
    timestamp = steady_clock::now();
}

ShaderProgramSource parseShader(const std::string &path) {
    std::ifstream stream(path);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream stringstream[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            if (type != ShaderType::NONE)
                stringstream[(int) type] << line << '\n';
        }
    }

    std::cout << "Successfully loaded Shaders from: " << path.c_str() << std::endl;

    return {stringstream[(int) ShaderType::VERTEX].str(),
            stringstream[(int) ShaderType::FRAGMENT].str()};
}

unsigned int createShader(const ShaderProgramSource &source) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, source.FragmentSource);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);


    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static unsigned int compileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!"
                  << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    std::cout << "Successfully compiled shader of type: " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << std::endl;

    return id;
}
