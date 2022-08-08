#include "config.hpp"
#include <glad/glad.h> // Important: Before GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <librealsense2/rs.hpp>

#include "Utilities/utils.hpp"
#include "Utilities/Window.hpp"
#include "Utilities/Texture.hpp"

int main() {
    Window window{"Intel Realsense", true};

    /******************* Intel Realsense ********************/
    rs2::pipeline pipe;
    pipe.start();
    rs2::frameset frames;

    rs2::colorizer colorizer;
    rs2::rates_printer printer;

    Texture depth_image, color_image;

    std::string str{};


    /******************* Main loop ********************/
    std::cout << "Starting main loop!" << std::endl;
    while (window) {

        frames = pipe.wait_for_frames();
        auto depth_frame = frames.get_depth_frame();
        auto depth = depth_frame.apply_filter(colorizer);
        float dist = depth_frame.get_distance(depth_frame.get_width() / 2, depth_frame.get_height() / 2);
        rs2::frame color = frames.get_color_frame();            // Find the color data

        // Render depth on to the first half of the screen and color on to the second
        depth_image.render(depth, {0, 0, window.width(), window.height()});


        str = "Distance: " + std::to_string(dist);
        draw_text(10, 10, str.c_str());


        while_timer();
    }

    std::cout << "Review logs ore close program by pressing Enter . . ." << std::endl;
//    std::cin.get();

    return 0;
}