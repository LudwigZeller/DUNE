#include "constants.hpp"
#include <glad/glad.h> // Important: Before GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <librealsense2/rs.hpp>

#include "utils.hpp"


int main() {
    assert_log(glfwInit(), "GLFW initialization failed");

    auto window = glfwCreateWindow(640, 280, "Intel Realsense", nullptr, nullptr);
    assert_log(window, "Window creation failed");

    glfwMakeContextCurrent(window);

    assert_log(gladLoadGL(), "OpenGL binding failed");
    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;

    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "Error " << error << ": " << description << std::endl;
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSwapInterval(1);

//    auto shader = parseShader("/shader/triangle.glsl");
//    auto program = createShader(shader);
//    glUseProgram(program);

    /******************* Intel Realsense ********************/

    while (!glfwWindowShouldClose(window)) {

        glfwSwapBuffers(window);
        while_timer();
    }

    glfwTerminate();
    return 0;
}