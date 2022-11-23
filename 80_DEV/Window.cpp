//
// Created by Ludwig Zeller on 08.08.2022.
//

#include "Window.hpp"

Window::Window(const char *title, bool fullscreen, const char *override) {
    /******************* Initialization ********************/
    this->_initialized = 0;
    assert_log(glfwInit(), "GLFW initialization failed");
    clog(info) << "GLFW initialized!\n" << std::endl;

    /******************* Window ********************/
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    clog(info) << "Connected monitors are: " << std::endl;
    GLFWmonitor *beamer = nullptr;
    for (int i = 0; i < count; ++i) {
        clog(info) << i << ": " << glfwGetMonitorName(monitors[i]) << std::endl;
        if (strcmp(glfwGetMonitorName(monitors[i]), override != nullptr ? override : DEFAULT_DEVICE_NAME) == 0)
            beamer = monitors[i];
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

    if(beamer) {
        int x,y;
        int w,h;
        glfwGetMonitorWorkarea(beamer, &x, &y, &w, &h);
        glfwSetWindowPos(window, x, y);
        glfwSetWindowSize(window, w, h);
    }

    assert_log(window, "Window creation failed");
    clog(info) << "Window creation succeeded!" << std::endl;
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

    glfwSetKeyCallback(window, Window::onKey);

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

    this->_initialized = 1;
}

Window::~Window() {
    glfwDestroyWindow(_window);
    _window_count--;
    if (!_window_count)
        glfwTerminate();
}

Window::operator bool() {
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

void Window::onKeyCustom(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE) {
        if (!glfwWindowShouldClose(window))
            clog(info) << "Closed by Escape" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if(key == GLFW_KEY_C && action == GLFW_RELEASE) {
        clog(warn) << "Switching stream.." << std::endl;
        _should_sw = true;
    }
    else if(key == GLFW_KEY_Q && action == GLFW_RELEASE) {
        _capture_flag = true;
    }
    else if(key == GLFW_KEY_E && action == GLFW_RELEASE)
    {
        if(file_exists("capture-0.dres"))
        {
            clog(warn) << "Switch to captures!" << std::endl;
            _show_captures = !_show_captures;
        }
    }
    else if(key == GLFW_KEY_R && action == GLFW_RELEASE && _show_captures)
    {
        _sel_capture++;
        if(!file_exists(std::string("capture-") + std::to_string(_sel_capture) + ".dres"))
            _sel_capture = 0;
    }
    else if(key == GLFW_KEY_F && action == GLFW_RELEASE && _show_captures)
    {
        _sel_capture--;
        if(!file_exists(std::string("capture-") + std::to_string(_sel_capture) + ".dres"))
            while(file_exists(std::string("capture-") + std::to_string(++_sel_capture) + ".dres"));
    }

}
