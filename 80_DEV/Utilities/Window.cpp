//
// Created by Ludwig Zeller on 08.08.2022.
//

#include "Window.hpp"

Window::Window(const char *title, bool fullscreen, const char *override) {
    /******************* Initialization ********************/
    assert_log(glfwInit(), "GLFW initialization failed");
    std::cout << "GLFW initialized!" << std::endl;

    /******************* Window ********************/
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    std::cout << "-----\nConnected monitors are: " << std::endl;
    GLFWmonitor *beamer = nullptr;
    for (int i = 0; i < count; ++i) {
        std::cout << i << ": " << glfwGetMonitorName(monitors[i]) << std::endl;
//        TODO:
        if (strcmp(glfwGetMonitorName(monitors[i]), override != nullptr ? override : DEFAULT_DEVICE_NAME) == 0)
            beamer = monitors[i];
    }
    std::cout << "-----" << std::endl;
    GLFWmonitor *monitor;
    if (beamer) {
        monitor = beamer;
    } else {
        std::cerr << "Didn't find Beamer, using primary monitor instead" << std::endl;
        monitor = glfwGetPrimaryMonitor();
    }
    GLFWwindow *window = glfwCreateWindow(glfwGetVideoMode(monitor)->width,
                                          glfwGetVideoMode(monitor)->height,
                                          title, fullscreen ? monitor : nullptr, nullptr);

    if(beamer) {
        int x,y;
        int w,h;
        glfwGetMonitorWorkarea(beamer, &x, &y, &w, &h);
        glfwSetWindowPos(window, x, y);
        glfwSetWindowSize(window, w, h);
    }

    assert_log(window, "Window creation failed");
    std::cout << "Window creation succeeded!" << std::endl;

    glfwMakeContextCurrent(window);
    _window = window;
    _width = glfwGetVideoMode(monitor)->width;
    _height = glfwGetVideoMode(monitor)->height;
    _window_count++;

    /******************* OpenGL Bindings ********************/
    // Need to be loaded after Context
    assert_log(gladLoadGL(), "OpenGL binding failed");
    std::cout << "OpenGL bindings created!" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /******************* Configuration ********************/
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE) {
            if (!glfwWindowShouldClose(window))
                std::cout << "Closed by Escape" << std::endl;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });

    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "Error " << error << ": " << description << std::endl;
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSetMonitorCallback([](GLFWmonitor *monitor, int event) {
        if (event == GLFW_DISCONNECTED) {
            std::cerr << "Monitor Disconnected" << std::endl;
            //TODO: Handle error instead of running away (Minimize window on primary, tell user etc...)
            glfwTerminate();
            exit(-1);
        }
        if (event == GLFW_CONNECTED) {
            std::cout << "Monitor Connected" << std::endl;
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