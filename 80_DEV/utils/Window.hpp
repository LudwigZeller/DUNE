//
// Created by Ludwig Zeller on 08.08.2022.
//

#ifndef DEPTHCAMERA_WINDOW_HPP
#define DEPTHCAMERA_WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Utils.hpp"
#include <opencv2/core/mat.hpp>
#include <mutex>

#define NO_MONITOR_FIX 0
#define MONITOR_BEAMER_FIX 1
#define MONITOR_TOUCHDISPLAY_FIX 2

class Window {
public:
    bool wpressed = false;
    bool docapture = false;
    bool cooldown = false;
    explicit Window(const char *title, const int _type = 0);

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
    void onPressCuston(GLFWwindow *window, int button, int action, int mods);

    void render_matrix(cv::Mat &&matrix);

    inline int real_width() {int w; glfwGetWindowSize(_window, &w, NULL); return w;}
    inline int real_height() {int h; glfwGetWindowSize(_window, NULL, &h); return h;}
    inline void set_type(const int type) {this->m_type = type;}
    inline int get_dat() const {return this->m_data;}
    inline void set_dat(const int dat) {this->m_data = dat;}

    inline void bind_capture_adress(std::function<void()> fct)
    {
        fct_ptr = fct;
    }


private:
    GLFWwindow *_window;
    int _width;
    int _height;
    std::mutex m_draw_mutex;
    cv::Mat m_matrix;
    inline static int _window_count = 0;
    int m_type = 0;
    int m_data = 0;
    cv::Point2i swipe_pt;
    std::function<void()> fct_ptr;

    inline static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window *wnd = (Window*)glfwGetWindowUserPointer(window);
        wnd->onKeyCustom(window, key, scancode, action, mods);
    }

    inline static void onPress(GLFWwindow *window, int button, int action, int mods)
    {
        Window *wnd = (Window*)glfwGetWindowUserPointer(window);
        wnd->onPressCuston(window, button, action, mods);
    }
};


#endif //DEPTHCAMERA_WINDOW_HPP
