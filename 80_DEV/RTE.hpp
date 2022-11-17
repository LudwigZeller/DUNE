

#pragma once

#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>
#include <atomic>
#include "Window.hpp"
#include "config.hpp"

namespace RTE
{
    static rs2::pipeline pipe{};
    static cv::Mat write_matrix;
    static std::mutex write_matrix_mutex;
    static std::atomic<bool> prim_write_m = false;
    static cv::Mat depth_matrix;
    static std::mutex depth_matrix_mutex;
    static cv::Mat color_matrix;
    static std::mutex color_matrix_mutex;
    static cv::Mat write_history1;
    static cv::Mat write_history2;
    static cv::Mat write_history3;
    static cv::Mat xtra;
    static bool camera_connected = false;
    static bool window_open = false;
    static bool terminate_all = false;
    static int exit_code_pipeline = 0;
    static int do_color = false;
    static std::atomic<int> millis_pm = 0;
    static std::atomic<int> millis_rm = 0;
    static Window window{"DUNE", FULLSCREEN};
    static float resizewidth = 0.0f;
    static float resizeheight = 0.0f;
    static float resizex = 0.0f;
    static float resizey = 0.0f;
}
