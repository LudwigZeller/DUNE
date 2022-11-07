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

    const bool initialized() const
    {
        return _initialized > 0;
    }

    GLFWwindow *getwndptr() const
    {
        return _window;
    }

    void onKeyCustom(GLFWwindow *window, int key, int scancode, int action, int mods);

    inline bool getssw() {return _should_sw;}
    inline void resssw() {_should_sw = false;}
    inline int realwidth() {int w; glfwGetWindowSize(_window, &w, NULL); return w;}
    inline int realheight() {int h; glfwGetWindowSize(_window, NULL, &h); return h;}


private:
    GLFWwindow *_window;
    int _width;
    int _height;
    int _initialized = 0;
    bool _should_sw = false;
    inline static int _window_count = 0;

    inline static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window *wnd = (Window*)glfwGetWindowUserPointer(window);
        wnd->onKeyCustom(window, key, scancode, action, mods);
    }
};


#endif //DEPTHCAMERA_WINDOW_HPP
