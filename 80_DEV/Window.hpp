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
    explicit Window(const char *title, bool fullscreen = false, const char *override = nullptr);

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

    inline bool getssw() const {return _should_sw;}
    inline void resssw() {_should_sw = false;}
    inline bool getcapture() const {return _capture_flag;}
    inline void rescapture() {_capture_flag = false;}
    inline bool getshowcap() const {return _show_captures;}
    inline void setshowcap(const bool a) {_show_captures = a;}
    inline int getisels() const {return _sel_capture;}
    inline void setisels(const int a) {_sel_capture = a;}

    inline int realwidth() {int w; glfwGetWindowSize(_window, &w, NULL); return w;}
    inline int realheight() {int h; glfwGetWindowSize(_window, NULL, &h); return h;}


private:
    GLFWwindow *_window;
    int _width;
    int _height;
    int _initialized = 0;
    bool _should_sw = false;
    bool _capture_flag = false;
    bool _show_captures = false;
    int _sel_capture = 0;
    inline static int _window_count = 0;

    inline static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window *wnd = (Window*)glfwGetWindowUserPointer(window);
        wnd->onKeyCustom(window, key, scancode, action, mods);
    }
};


#endif //DEPTHCAMERA_WINDOW_HPP
