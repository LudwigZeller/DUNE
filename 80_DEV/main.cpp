/****************************************\
|      ____    _  _   _   _   ____       |
|     |  _ \  | || | | \ | | |  __|      |
|     | | \ \ | || | |  \| | | |_        |
|     | | | | | || | |     | |  _|       |
|     | |_/ / | || | | |\  | | |__       |
|     |____/  \____/ |_| \_| |____|      |
|                                        |
|   DUNE - Sandbox Depth Visualizer      |
|  A project by                          |
|  Ludwig Zeller and David Schoosleitner |
|                                        |
|****************************************|
|                                        |
| @file main.cpp                         |
|                                        |
| This file is responsible for:          |
|  - Pipeline and loop                   |
| This file depends on:                  |
|  - GLAD                                |
|  - glfw3 Library                       |
|  - OpenCV Library                      |
|  - Intel(R) Real Sense 2 Library       |
|  - Local Utilities                     |
|                                        |
\****************************************/


/****************************************\
|               INCLUDES                 |
\****************************************/

/*****    DYNAMIC LIBRARY INCLUDE   *****/
#include <glad/glad.h>                              //< OpenGL function loader; Important: Before GLFW
#include <GLFW/glfw3.h>                             //< OpenGL
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

/*****        LOCAL INCLUDE         *****/
#include "config.hpp"
#include "Utilities/utils.hpp"
#include "Utilities/Window.hpp"
#include "Utilities/Texture.hpp"

/*****           MISC               *****/
#include <iostream>

int main() try {
    std::cout << "Starting DepthWatch " << VERSION << std::endl;

    Window window{"DepthWatch", FULLSCREEN};

    //******************* Intel Realsense ********************
    rs2::stream_profile stream_profile{};
    rs2::pipeline pipe{};
    rs2::config config{};

    //https://community.intel.com/t5/Items-with-no-label/Problem-with-the-Realsense-SDK-2-0-and-OpenCV/m-p/605730
//    config.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);
//    config.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
//    pipe.start(config);
    pipe.start();

    rs2::colorizer colorizer{9};
    rs2::rates_printer printer{};
    rs2::hole_filling_filter filler{0};
    rs2::spatial_filter spatial{0.5, 20, 2, 2};
    rs2::temporal_filter temporal{};

    rs2::align align_to(RS2_STREAM_COLOR);

    std::string str{};
    unsigned int cycle_counter = 0;

    std::vector<cv::Vec3f> circles;


    //******************* Init ********************
    rs2::frameset frames = pipe.wait_for_frames();
    Quadrangle bounds = check_bounds(frames.get_color_frame(), circles);

    //******************* Main loop ********************
    std::cout << "Starting main loop!" << std::endl;
    while (window) {

        frames = pipe.wait_for_frames();
        frames = align_to.process(frames); // High performance impact

        auto depth_frame = frames.get_depth_frame();
        auto depth = frames
                .apply_filter(temporal)
//                .apply_filter(filler)
                .apply_filter(colorizer);

        //auto lol = frames.get_color_frame();

        cv::Mat depth_matrix(cv::Size(depth_frame.get_width(), depth_frame.get_height()), CV_8UC3,
                             (void *) depth.get_data(),
                             cv::Mat::AUTO_STEP);
//        cv::Mat depth_matrix = frame_to_mat(depth);

/*        for (auto &i: circles) {
            cv::Vec3i c = i;
            cv::Point center = cv::Point(c[0], c[1]);
            // circle center
            circle(depth_matrix, center, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
            // circle outline
            int radius = c[2];
            circle(depth_matrix, center, radius, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        }*/

        draw_quadrangle(depth_matrix, bounds);
        // draw circles


        draw_frame(cv::Size((int) window.width(), (int) window.height()), depth_matrix);


#if DEBUG_ON_SCREEN
        float dist = depth_frame.get_distance(depth_frame.get_width() / 2, depth_frame.get_height() / 2);
        str = "Distance: " + std::to_string(dist);
        draw_text_debug(10, 10, str.c_str());
#endif
        while_timer();

        //This takes roughly 5000ms every time cycle_counter reaches CALIBRATION_LOOP_THRESHOLD
        //cycle_counter++;
        //if (cycle_counter >= CALIBRATION_LOOP_THRESHOLD) {;            // Find the color data
        //    cycle_counter = 0;
        //    bounds = check_bounds(frames.get_color_frame(), circles);
        //}
    }



    /* Clean Up */
    window.close();
    while (window);
//    window.~Window(); // Explicit Deconstructor call
//    std::cout << "Review logs ore close program by pressing Enter . . ." << std::endl;
//    std << cin.get();

    return 0;
}
catch (const rs2::error &e) {
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    "
              << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
