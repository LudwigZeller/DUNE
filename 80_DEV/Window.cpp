//
// Created by Ludwig Zeller on 08.08.2022.
//

#include "Window.hpp"
#include "config.hpp"
#include <cstring>
#include <fstream>


Window::Window(const char *title, const int _type) : m_type(_type) {
    /******************* Initialization ********************/
    assert_log(glfwInit(), "GLFW initialization failed");
    clog(info) << "GLFW initialized!\n" << std::endl;

    /******************* Window ********************/
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    clog(info) << "Connected monitors are: " << std::endl;
    GLFWmonitor *beamer = nullptr;
    if(_type == MONITOR_BEAMER_FIX)
        for (int i = 0; i < count; ++i) {
            clog(info) << i << ": " << glfwGetMonitorName(monitors[i]) << std::endl;
            int mwmm, mhmm;
            glfwGetMonitorPhysicalSize(monitors[i], &mwmm, &mhmm);
            if(mwmm == DEFAULT_DEVICE_PWIDTH && mhmm == DEFAULT_DEVICE_PHEIGHT)
            {
                beamer = monitors[i];
                break;
            }
        }
    else if(_type == MONITOR_TOUCHDISPLAY_FIX)
        for (int i = 0; i < count; ++i) {
            clog(info) << i << ": " << glfwGetMonitorName(monitors[i]) << std::endl;
            int mwmm, mhmm;
            glfwGetMonitorPhysicalSize(monitors[i], &mwmm, &mhmm);
            if(mwmm == TOUCH_DEVICE_PWIDTH && mhmm == TOUCH_DEVICE_PHEIGHT)
            {
                beamer = monitors[i];
                break;
            }
        }
    clog(info) << "----------\n" << std::endl;
    GLFWmonitor *monitor;
    if (beamer) {
        monitor = beamer;
    } else {
        clog(warn) << "Didn't find Beamer, using primary monitor instead" << std::endl;
        monitor = glfwGetPrimaryMonitor();
    }
    GLFWwindow *window = glfwCreateWindow(glfwGetVideoMode(monitor)->width,
                                          glfwGetVideoMode(monitor)->height,
                                          title, nullptr, nullptr);

    if (beamer) {
        int x, y;
        int w, h;
        glfwGetMonitorWorkarea(beamer, &x, &y, &w, &h);
        glfwSetWindowPos(window, x, y);
        glfwSetWindowSize(window, w, h);
    }

    assert_log(window, "Window creation failed");
    clog(info) << "Window creation succeeded! " << title << std::endl;
    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);
    _window = window;
    _width = glfwGetVideoMode(monitor)->width;
    _height = glfwGetVideoMode(monitor)->height;
    _window_count++;

    /******************* OpenGL Bindings ********************/
    // Need to be loaded after Context
    assert_log(gladLoadGL(), "OpenGL binding failed");
    clog(info) << "OpenGL bindings created!" << std::endl;
    clog(info) << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /******************* Configuration ********************/
    glfwSwapInterval(1);
    glfwSetWindowAttrib(window, GLFW_DECORATED, FULLSCREEN ? GLFW_FALSE : GLFW_TRUE);

#if WEB_UI
    glfwSetKeyCallback(window, Window::onKey);
#else
    if(_type == MONITOR_TOUCHDISPLAY_FIX)
    {
        glfwSetKeyCallback(window, Window::onKey);
        glfwSetMouseButtonCallback(window, Window::onPress);
    }
#endif

    glfwSetErrorCallback([](int error, const char *description) {
        clog(err) << "GLFW Error callback: " << error << ": " << description << std::endl;
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSetMonitorCallback([](GLFWmonitor *monitor, int event) {
        if (event == GLFW_DISCONNECTED) {
            clog(err) << "Monitor Disconnected" << std::endl;
            //TODO: Handle error instead of running away (Minimize window on primary, tell user etc...)
            glfwTerminate();
            exit(-1);
        }
        if (event == GLFW_CONNECTED) {
            clog(warn) << "Monitor Connected" << std::endl;
            // TODO: Check if connected monitor is beamer and switch to it
        }
    });
}

Window::~Window() {
    glfwDestroyWindow(_window);
    _window_count--;
    if (!_window_count)
        glfwTerminate();
}

Window::operator bool() {
    glfwMakeContextCurrent(this->_window);
    {
        std::lock_guard<std::mutex> lock(m_draw_mutex);
        draw_frame(cv::Size((int) this->width(), (int) this->height()), m_matrix);
    }
    glPopMatrix();
    glfwSwapBuffers(_window);

    auto res = !glfwWindowShouldClose(_window);

    glfwPollEvents();
    glfwGetFramebufferSize(_window, &_width, &_height);

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, _width, _height);

    glPushMatrix();
    glfwGetWindowSize(_window, &_width, &_height);
    glOrtho(0, _width, _height, 0, -1, +1);

    return res;
}

#include "Pipeline/CalibRTE.hpp"

void Window::onKeyCustom(GLFWwindow *window, int key, int scancode, int action, int mods) {
    static bool shift_prsd = false;
    if (key == GLFW_KEY_ESCAPE) {
        if (!glfwWindowShouldClose(window))
            clog(info) << "Closed by Escape" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if(key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        this->wpressed = true;
    }
    else if(key == GLFW_KEY_LEFT_SHIFT)
    {
        if(action == GLFW_PRESS)
        {
            shift_prsd = true;
        }
        else if(action == GLFW_RELEASE)
        {
            shift_prsd = false;
        }
    }
    else if(key == GLFW_KEY_UP && action == GLFW_REPEAT)
    {
        scalar_kernel.width += shift_prsd ? -1 : 1;
        goto translation_scalar_jmp;
    }
    else if(key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
    {
        scalar_kernel.height += shift_prsd ? -1 : 1;
        goto translation_scalar_jmp;
    }
    else if(key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
    {
        translation_vec.x += shift_prsd ? -1 : 1;
        goto translation_scalar_jmp;
    }
    else if(key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)
    {
        translation_vec.y += shift_prsd ? -1 : 1;
        goto translation_scalar_jmp;
    }
    else if(key == GLFW_KEY_C && action == GLFW_RELEASE)
    {
        this->fct_ptr();
    }
    return;
translation_scalar_jmp:
    clog(warn) << "Changed transformation properties!\n scalar width = " << (double) scalar_kernel.width / (double) STREAM_WIDTH <<
    "\n scalar height = " << (double) scalar_kernel.height / (double) STREAM_HEIGHT <<
    "\n translation x = " << translation_vec.x <<
    "\n translation y = " << translation_vec.y << std::endl;
}

void Window::render_matrix(cv::Mat &&matrix) {
    std::lock_guard<std::mutex> lock(m_draw_mutex);
    m_matrix = std::move(matrix);
}

void Window::onPressCuston(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && !this->cooldown)
    {
        double posx, posy;
        glfwGetCursorPos(window, &posx, &posy);
        this->swipe_pt = {(int)posx, (int)posy};
    }
    else if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE && !this->cooldown)
    {
        double posx, posy;
        glfwGetCursorPos(window, &posx, &posy);
        posx -= swipe_pt.x;

        if(posx < -10)
        {
            this->m_data--;
            if(this->m_data < 0)
            {
                this->m_data = 3;
            }
        }
        else if(posx > 10)
        {
            this->m_data++;
            this->m_data %= 4;
        }
    }
}
