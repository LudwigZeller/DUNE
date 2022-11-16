/*****************************************
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
| @file DrsGrabberManager.cpp            |
| @author David Schoosleitner            |
| @date Nov. 7th 2022                    |
|                                        |
| This file is responsible for:          |
|  - Grabbing rs frames and processing   |
| This file depends on:                  |
|  - Managers                            |
|  - RTE                                 |
|  - Local utilities                     |
|****************************************|
|                                        |
| USED LIBRARIES ARE PROPERTY OF THEIR   |
| CORRESPECTIVE OWNERS.                  |
|                                        |
| SEE COPYRIGHT.md IN PROJECT'S ROOT     |
| DIRECTORY, AS WELL AS                  |
| INDIVIDUAL LICENSES AND TOS.           |
|                                        |
*****************************************/

#pragma once

/*****************************************
|               INCLUDES                 |
*****************************************/
#include "DManager.hpp"
#include "../config.hpp"
#include "../RTE.hpp"
#include "../utils.hpp"

using namespace std::chrono;


/**********************
 * NAMESPACE DManager *
 **********************
 * @namespace DManager
 * @brief Contains multithreading-related
 * @brief tools for DUNE
 **********************/
namespace DManager
{


/**********************
 * CLASS DrsGrabberManager : public DManager
 **********************
 * @class DrsGrabberManager : public DManager
 * @brief Responsible for getting depth and color frame of Realsense Device.
 * @brief Performs temporal and filter processing. Saves elaborated images into RTE.
 **********************/
class DrsGrabberManager : public DManager
{
public:

    /**
     * @fn DrsGrabberManager()
     * @brief Construct a new Drs Grabber Manager object.
     * @brief DManagerFlags are set by default.
     */
    DrsGrabberManager() : DManager(DManagerFlags{
        .can_skip = true,
        .log_lag = DEBUG_ON_SCREEN,
        .target_period = milliseconds(int(1000.0f / CPS)),
        .delay_target_period = milliseconds(int(500.0f / CPS)),
        .log_ela = false,
        .name = "rs Grabber Manager",
        .adress_millis = &RTE::millis_rm
    })
    {
    }

    /*****       CLASS MEMBERS       *****/

    //!! RS pipeline members
    rs2::pipeline pipe{};                   //< Pipeline object
    rs2::align align_to{RS2_STREAM_COLOR};  //< Align depth frame to color frame
    rs2::temporal_filter temporal{};        //< Temporal filter stabilizes depth image
    rs2::colorizer colorizer{9};            //< Colorizer filter converts depth image into hue
    rs2::config config{};                   //< Configuration of pipeline enables devices and streams
    rs2::frameset frames;                   //< Capture object
    rs2::pipeline_profile profile;          //< Profile of current pipeline

    //!! Misc. Members
    volatile int count_errors = 0;          //< Termination condition (per error)
    volatile int count_checkup = 0;         //< Device scan condition (per tick)

    /*****       CLASS FUNCTIONS       *****/

    /**
     * @fn inline static bool camera_connected()
     * @brief Util gets if device is connected 
     * @return true - realsense camera is connected
     */
    inline static bool camera_connected()
    {
        rs2::context ctx;
        auto device_list = ctx.query_devices();
        if(device_list.size() < 1ULL)
        {
            //No device found
            return false;
        }
        return true;
    }

    /**
     * @fn void end_stream()
     * @brief Non-DManager function ends rs stream if possible. Called by DControlManager
     */
    void end_stream() try
    {
        clog(info) << "Ending rs Stream..." << std::endl;
        pipe.stop();
    } catch(const rs2::error &e)
    {
        //! Nothing to worry about
        clog(warn) << "pipe.stop() harmless error: " << e.what() << std::endl;
    }

    /**
     * @fn virtual void init_user() override
     * @brief Overridden initialization of Pipeline Manager.
     */
    virtual void init_user() override
    {
        clog(info) << "Starting rs Grabber Manager" << std::endl;
        if(!camera_connected())
            clog(warn) << "No camera connected!" << std::endl;
    }

    /**
     * @fn virtual void run_user() override
     * @brief Cyclic routine of Grabber Manager.
     * @brief Grabs depth and color frame from Realsense camera. Saves to RTE.
     */
    /// @brief 
    virtual void run_user() override try
    {
        //!! Routine 1: No camera has been connected
        if(!RTE::camera_connected)
        {
            //!! Get device
            rs2::context ctx;
            auto device_list = ctx.query_devices();
            if(device_list.size() < 1ULL)
            {
                //No device found
                return;
            }

            //! Routine camera has been found
            clog(info) << "Camera found! " <<
                device_list.front().get_info(RS2_CAMERA_INFO_NAME) <<
                std::endl;

            //!! Standard configuration for RS D435i
            config.enable_device(device_list.front().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
            config.enable_stream(RS2_STREAM_DEPTH, 480, 270, RS2_FORMAT_Z16, 15);
            config.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);

            //!! Trigger Routine 2
            profile = pipe.start(config);
            if(profile)
                RTE::camera_connected = true;
        }

        //! Check if the device has been disconnected every 50 ticks
        if(++count_checkup >= 50)
        {
            if(!camera_connected())
            {
                clog(warn) << "Camera disconnected! Stopping stream and resetting config." << std::endl;
                pipe.stop();
                config.disable_all_streams();

                //! Trigger Routine 1
                RTE::camera_connected = false;
                count_checkup = 0;
                return;
            }

            count_checkup = 0;
        }

        //!! Routine 2: Camera is connected and can grab frames from rs pipeline
        if(pipe.poll_for_frames(&frames))
        {
            RTE::depth_matrix_mutex.lock();
            RTE::color_matrix_mutex.lock();
            RTE::write_matrix_mutex.lock();
            /***** GRAB FRAMES *****/
            frames = align_to.process(frames);
            auto color_frame = frames.get_color_frame();
            auto depth_frame = frames.get_depth_frame();

            /***** PROCESS FRAMES *****/
            auto depth = frames
                    .apply_filter(temporal);
            //      .apply_filter(filler)
            //        .apply_filter(colorizer);

            /***** CONVERT FRAMES *****/
            /**** WRITE FRAME ****/
            cv::Mat mat(
                cv::Size(depth_frame.get_width(), depth_frame.get_height()),
                CV_8UC3,
                (void *) depth.get_data(),
                cv::Mat::AUTO_STEP
            );
            
            
            RTE::write_matrix = mat;
            RTE::write_matrix_mutex.unlock();
            /**** DEPTH FRAME ****/
            RTE::xtra = frame_to_mat(frames.get_depth_frame());
            cv::Mat dmat = depth_frame_to_meters(frames.get_depth_frame());

            
            RTE::depth_matrix = dmat;
            RTE::depth_matrix_mutex.unlock();

            /**** COLOR FRAME ****/
            cv::Mat cmat(
                cv::Size(color_frame.get_width(), color_frame.get_height()),
                CV_8UC3,
                (void *) color_frame.get_data(),
                cv::Mat::AUTO_STEP
            );

            
            RTE::color_matrix = cmat;
            RTE::color_matrix_mutex.unlock();

            RTE::resizewidth = RTE::window.realwidth();
            RTE::resizeheight = RTE::window.realwidth() * (float) frames.get_depth_frame().get_height() / (float) frames.get_depth_frame().get_width();
            RTE::resizex = 0.0f;
            RTE::resizey = abs(RTE::resizeheight - RTE::window.realheight()) / 2;
        }
        //! Reset counter
        count_errors = 0;
    } catch(const rs2::error &e)
    {
        clog(err) << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    "
              << e.what() << std::endl;
        if(count_errors++ > 9)
        {
            clog(err) << "Forced termination after 10 rs2::error's" << std::endl;
            RTE::terminate_all = true;
            RTE::exit_code_pipeline = 1;
        }
    } catch(const std::exception &e)
    {
        clog(err) << "Standard error " << e.what() << std::endl;
        if(count_errors++ > 9)
        {
            clog(err) << "Forced termination after 10 std::exception's" << std::endl;
            RTE::terminate_all = true;
            RTE::exit_code_pipeline = 1;
        }
    }
};

};
