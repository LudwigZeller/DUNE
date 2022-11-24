//
// Created by Ludwig Zeller on 08.08.2022.
//

#ifndef DEPTHCAMERA_WINDOW_HPP
#define DEPTHCAMERA_WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils.hpp"
#include <opencv2/core/mat.hpp>
#include <mutex>

class Window {
public:
    explicit Window(const char *title, bool fullscreen = false, const char *device = nullptr);

    ~Window();

    void close() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }
    [[nodiscard]] float width() const { return (float)_width; }
    [[nodiscard]] float height() const { return (float)_height; }

    explicit operator bool();

    GLFWwindow *get_window_ptr() const
    {
        return _window;
    }

    void onKeyCustom(GLFWwindow *window, int key, int scancode, int action, int mods);

    void render_matrix(cv::Mat &&matrix);

    inline int real_width() {int w; glfwGetWindowSize(_window, &w, NULL); return w;}
    inline int real_height() {int h; glfwGetWindowSize(_window, NULL, &h); return h;}


private:
    GLFWwindow *_window;
    int _width;
    int _height;
    std::mutex m_draw_mutex;
    cv::Mat m_matrix;
    inline static int _window_count = 0;

    inline static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window *wnd = (Window*)glfwGetWindowUserPointer(window);
        wnd->onKeyCustom(window, key, scancode, action, mods);
    }
};


#endif //DEPTHCAMERA_WINDOW_HPP
