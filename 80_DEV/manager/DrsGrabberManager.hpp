#include "DManager.hpp"
#include "../config.hpp"
#include "../RTE.hpp"

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

using namespace std::chrono;

namespace DManager
{

class DrsGrabberManager : public DManager
{
public:
    DrsGrabberManager() : DManager(DManagerFlags{
        .can_skip = false,
        .log_lag = DEBUG_ON_SCREEN,
        .target_period = milliseconds(int(1000.0f / FPS)),
        .delay_target_period = milliseconds(int(500.0f / FPS)),
        .log_ela = false,
        .name = "rs Grabber Manager"
    })
    {
    }

    rs2::pipeline pipe{};
    rs2::align align_to{RS2_STREAM_COLOR};
    rs2::temporal_filter temporal{};
    rs2::colorizer colorizer{9};
    rs2::config config{};
    rs2::frameset frames;
    rs2::pipeline_profile profile;
    volatile int count_errors = 0;

    virtual void init_user()
    {
        std::cout << "Starting rs Grabber Manager" << std::endl;
        if(!camera_connected())
            std::cout << "No camera connected!" << std::endl;
    }

    virtual void run_user() try
    {
        //Do: Get device
        if(!RTE::camera_connected)
        {
            rs2::context ctx;
            auto device_list = ctx.query_devices();
            if(device_list.size() < 1ULL)
            {
                //No device found
                return;
            }

            std::cout << "Camera found!\n" <<
                device_list.front().get_info(RS2_CAMERA_INFO_NAME) <<
                std::endl;

            if(pipe.get_active_profile())
            {
                pipe.stop();
            }

            config.enable_device(device_list.front().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
            config.enable_stream(RS2_STREAM_DEPTH, 480, 270, RS2_FORMAT_Z16, 15);
            config.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);

            profile = pipe.start(config);
            if(profile)
                RTE::camera_connected = true;
        }

        if(pipe.poll_for_frames(&frames))
        {
            frames = align_to.process(frames);
            auto depth_frame = frames.get_depth_frame();

            auto depth = frames
                    .apply_filter(temporal)
            //      .apply_filter(filler)
                    .apply_filter(colorizer);

            RTE::window_out_matrix = cv::Mat(
                cv::Size(depth_frame.get_width(), depth_frame.get_height()),
                CV_8UC3,
                (void *) depth.get_data(),
                cv::Mat::AUTO_STEP
            );
        }
        count_errors = 0;
    }
    catch(const rs2::error &e)
    {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    "
              << e.what() << std::endl;
        if(count_errors++ > 9)
        {
            terminate();
            RTE::terminate_all = true;
            RTE::exit_code_pipeline = 1;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Standard error " << e.what() << std::endl;
        if(count_errors++ > 9)
        {
            terminate();
            RTE::terminate_all = true;
            RTE::exit_code_pipeline = 1;
        }
    }
};

};
