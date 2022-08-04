#include "constants.hpp"
#include <glad/glad.h> // Important: Before GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <librealsense2/rs.hpp>
//#include <ft2build.h>
//#include FT_FREETYPE_H

#include "utils.hpp"


int main() {
    /******************* Initialization ********************/
    assert_log(glfwInit(), "GLFW initialization failed");

//    FT_Library ft;
//    assert_log(!(FT_Init_FreeType(&ft)), "Could not init FreeType Library");
//
//    FT_Face face;
//    assert_log(!(FT_New_Face(ft, "fonts/arial.ttf", 0, &face)), "Failed to load font");

    /******************* Window ********************/
    int count;
    GLFWmonitor **monitor = glfwGetMonitors(&count);
    std::cout << "-----\nConnected monitors are: " << std::endl;
    GLFWmonitor *beamer = nullptr;
    for (int i = 0; i < count; ++i) {
        std::cout << i << ": " << glfwGetMonitorName(monitor[i]) << std::endl;
//        TODO:
//        if (glfwGetMonitorName(monitor[i]) == "/*BEAMER*/")
//            beamer = monitor[i];
    }
    std::cout << "-----" << std::endl;
    if (!beamer) {
        std::cerr << "Didn't find Beamer, using primary monitor instead" << std::endl;
    }
    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    GLFWwindow *window = glfwCreateWindow(glfwGetVideoMode(primary_monitor)->width,
                                          glfwGetVideoMode(primary_monitor)->height,
                                          "Intel Realsense", beamer ? beamer : primary_monitor, nullptr);
    assert_log(window, "Window creation failed");

    glfwMakeContextCurrent(window);

    /******************* OpenGL Bindings ********************/
    // Need to be loaded after Context
    assert_log(gladLoadGL(), "OpenGL binding failed");
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /******************* Callbacks ********************/
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

    glfwSwapInterval(1);

    /******************* Intel Realsense ********************/
    rs2::pipeline pipe;
    pipe.start();
    rs2::frameset frames;
    std::string str{};
    /******************* Main loop ********************/
    while (!glfwWindowShouldClose(window)) {

        frames = pipe.wait_for_frames();
        auto depth = frames.get_depth_frame();
        float dist = depth.get_distance(depth.get_width() / 2, depth.get_height() / 2);
        str = "Distance: " + std::to_string(dist);
        draw_text(100, 100, "TEST");
        std::cout << str << std::endl;

        glfwPollEvents();
        glfwSwapBuffers(window);
        while_timer();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Review logs ore close program by pressing Enter . . ." << std::endl;
//    std::cin.get();

    return 0;
}