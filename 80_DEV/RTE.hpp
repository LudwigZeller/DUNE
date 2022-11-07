

#pragma once

#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>
#include "Utilities/Window.hpp"
#include "config.hpp"

using namespace DManager;

namespace RTE
{
    static cv::Mat window_out_matrix;
    static cv::Mat color_matrix;
    static bool camera_connected = false;
    static bool window_open = false;
    static bool terminate_all = false;
    static int exit_code_pipeline = 0;
    static int do_color = false;
    static int millis_pm = 0;
    static int millis_rm = 0;
    static Window window{"DUNE", FULLSCREEN};
}
