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
| @author Ludwig Zeller                  |
|                                        |
| This file is responsible for:          |
|  - Pipeline and loop                   |
| This file depends on:                  |
|  - GLAD Library                        |
|  - glfw3 Library                       |
|  - OpenCV Library                      |
|  - Intel(R) Real Sense 2 Library       |
|  - Local Utilities                     |
|                                        |
|****************************************|
|                                        |
| USED LIBRARIES ARE PROPERTY OF THEIR   |
| CORRESPECTIVE OWNERS.                  |
|                                        |
| SEE COPYRIGHT.md IN PROJECT'S ROOT     |
| DIRECTORY, AS WELL AS                  |
| INDIVIDUAL LICENSES AND TOS.           |
|                                        |
\****************************************/


/****************************************\
|               INCLUDES                 |
\****************************************/

/*****    DYNAMIC LIBRARY INCLUDE   *****/
#include <glad/glad.h>                              //<  OpenGL function loader; Important: Before GLFW
#include <GLFW/glfw3.h>                             //<  OpenGL library for window and input
#include <librealsense2/rs.hpp>                     //<  Intel(R) RealSenseTM 3D-Camera SDK
#include <opencv2/opencv.hpp>                       //!< OpenCV library 
#include <opencv2/objdetect.hpp>

/*****        LOCAL INCLUDE         *****/
#include "config.hpp"
#include "Utilities/utils.hpp"
#include "Utilities/Window.hpp"
#include "Utilities/Texture.hpp"

/*****           MISC               *****/
#include <iostream>

typedef uchar Pixel;
/****************************************\
|               MAIN FN                  |
\****************************************/

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */
int main(int argc, char **argv) try
{
    //!!Parse arguments
    char *device_name = nullptr;        //< Display device name

    for(int i = 0; i < argc; ++i)
    {
        if((strlen(argv[i]) > 2) && (argv[i][0] == '-') && (argv[i][1] == 'm'))
        {
            device_name = argv[i] + 2;
        }
    }
    

    //!! Check for connected hardware
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    if(device_list.size() < 1ULL)
    {
        std::cerr << "No devices connected!\nMake sure to plug in the Camera!" << std::endl;
        return -1;
    }

    std::cout << "Starting DepthWatch " << VERSION << std::endl;

    //!! Intel(R) RealsenseTM init
    rs2::pipeline pipe{};
    rs2::stream_profile stream_profile{};
    rs2::config config{};

    //!! REF:
    // https://community.intel.com/t5/Items-with-no-label/Problem-with-the-Realsense-SDK-2-0-and-OpenCV/m-p/605730
    config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    config.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 10);
    if(!config.can_resolve(pipe))
    {
        std::cerr << "Fatal error! Config cannot be resolved!" << std::endl;
        return -1;
    }
     
    (void)pipe.start(config);

    //! Pipeline start with default configuration
    //(void)pipe.start();
    std::cout << "reached here" << std::endl;

    //!! Integrated filters init
    rs2::colorizer colorizer{9};
    rs2::temporal_filter temporal{};
    //  rs2::rates_printer printer{};
    //  rs2::hole_filling_filter filler{0};
    //  rs2::spatial_filter spatial{0.5, 20, 2, 2};

    //! Depth to Color alignment
    rs2::align align_to(RS2_STREAM_COLOR);
    std::cout << "and here" << std::endl;

    //! Window Creation is controlled by Window Util
    std::cout << "Launch window." << std::endl;
    Window window{"DepthWatch", FULLSCREEN, device_name};

    //!! Utils
    std::string str{};
    unsigned int cycle_counter = 0;

    //std::vector<cv::Vec3f> circles;


    //******************* Init ********************
    rs2::frameset frames = pipe.wait_for_frames();
    frames = align_to.process(frames);
    //Quadrangle bounds = check_bounds(frames.get_color_frame(), circles);

    float resizewidth = window.width();
    float resizeheight = window.width() * (float) frames.get_depth_frame().get_height() / (float) frames.get_depth_frame().get_width();
    float resizex = 0.0f;
    float resizey = abs(resizeheight - window.height()) / 2;


    //******************* Viewport ********************
    //set_viewport({.x = 0, .y = 0, .w = 100, .h = 100});

    //******************* Main loop ********************
    std::cout << "Starting main loop!" << std::endl;
    while (window) {

        //! Pipeline get frames and align depth image to color images
        frames = pipe.wait_for_frames();
        frames = align_to.process(frames); //< High performance impact

        auto color_frame = frames.get_color_frame();
        auto depth_frame = frames.get_depth_frame();
        auto depth = frames
                .apply_filter(temporal)
        //      .apply_filter(filler)
                .apply_filter(colorizer);

        cv::Mat depth_matrix(
            cv::Size(depth_frame.get_width(), depth_frame.get_height()),
            CV_8UC3,
            (void *) depth.get_data(),
            cv::Mat::AUTO_STEP
        );

        cv::Mat color_matrix(
            cv::Size(color_frame.get_width(), color_frame.get_height()),
            CV_8UC3,
            (void *) color_frame.get_data(),
            cv::Mat::AUTO_STEP
        );

        cv::Mat *dm = &depth_matrix;

        depth_matrix.forEach<Pixel>([dm, color_matrix](Pixel &p, const int *pos)
        {
            cv::Vec3b &depth = (*dm).at<cv::Vec3b>(pos[0],pos[1]);
            int mode = 
                (depth[0] > 150) ? (
                    (depth[1] > 150) ? (1)
                    :(depth[2] > 150) ? (1)
                    : (0)
                ) : (depth[1] > 150) ? (
                    (depth[2] > 150) ? (1)
                    : (0)
                ) : (0);
            if(mode && (((pos[0] + pos[1]) % 16) > 7) || !mode && ((abs(pos[0] - pos[1]) % 16) > 7)) {
                depth = color_matrix.at<cv::Vec3b>(pos[0],pos[1]);
            }
        });

        //  for (auto &i: circles) {
        //      cv::Vec3i c = i;
        //      cv::Point center = cv::Point(c[0], c[1]);
        //      // circle center
        //      circle(depth_matrix, center, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        //      // circle outline
        //      int radius = c[2];
        //      circle(depth_matrix, center, radius, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        //  }

        //  draw_quadrangle(depth_matrix, bounds);
        //  draw circles

        glViewport(resizex, resizey, resizewidth, resizeheight);
        glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
        draw_frame(cv::Size((int) window.width(), (int) window.height()), depth_matrix);

        //!! Compiler-level configurable on-screen debug information
        #if DEBUG_ON_SCREEN
        float dist = depth_frame.get_distance(depth_frame.get_width() / 2, depth_frame.get_height() / 2);
        str = "Distance: " + std::to_string(dist);
        draw_text_debug(10, 10, str.c_str());
        #endif
        while_timer();

        //!! ISSUE:
        // This takes roughly 5000ms every time cycle_counter reaches CALIBRATION_LOOP_THRESHOLD
        // check_bounds seems to contain heavy-load processes and should be changed entirely or be run
        // on a seperate, monitored thread.
        //  cycle_counter++;
        //  if (cycle_counter >= CALIBRATION_LOOP_THRESHOLD) {;            // Find the color data
        //      cycle_counter = 0;
        //      bounds = check_bounds(frames.get_color_frame(), circles);
        //  }
    }

    //!! Clean up on window should close call
    window.close();
    while (window);
    //  window.~Window(); // Explicit Deconstructor call
    //  std::cout << "Review logs or close program by pressing Enter . . ." << std::endl;
    //  std << cin.get();

    return 0;
}
catch (const rs2::error &e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    "
              << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
