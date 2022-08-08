//
// Created by Ludwig Zeller on 08.08.2022.
//

#ifndef DEPTHCAMERA_WINDOW_HPP
#define DEPTHCAMERA_WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils.hpp"

class Window {
public:
    explicit Window(const char *title, bool fullscreen = false);

    ~Window();

    void close() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }
    [[nodiscard]] float width() const { return (float)_width; }
    [[nodiscard]] float height() const { return (float)_height; }

    explicit operator bool();


private:
    GLFWwindow *_window;
    int _width;
    int _height;
    inline static int _window_count = 0;
};


#endif //DEPTHCAMERA_WINDOW_HPP
