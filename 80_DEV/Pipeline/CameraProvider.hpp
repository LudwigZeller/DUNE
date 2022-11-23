#ifndef DUNE_PIPELINE_CAMERAPROVIDER_HPP
#define DUNE_PIPELINE_CAMERAPROVIDER_HPP

#include "Provider.hpp"
#include <librealsense2/rs.hpp>
#include <chrono>
#include <thread>
#include <utility>

class CameraProvider : public Provider {
    rs2::pipeline m_pipe{};

public:
    explicit CameraProvider(std::string id) : Provider(std::move(id)) {
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
        device_list.front().get_info(RS2_CAMERA_INFO_NAME) << std::endl;
        //!! Standard configuration for RS D435i
        rs2::config config{};
        config.enable_device(device_list.front().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
        config.enable_stream(RS2_STREAM_DEPTH, 480, 270, RS2_FORMAT_Z16, 15);
        config.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);
        //!! Trigger Routine 2
        m_pipe.start(config);
    };

    ~CameraProvider() override{
        m_pipe.stop();
    }

    void work() override {
        rs2::frameset frames = m_pipe.wait_for_frames();
        
        rs2::temporal_filter temporal{};        //< Temporal filter stabilizes depth image

        /***** GRAB FRAMES *****/
        auto align = rs2::align{RS2_STREAM_COLOR};
        frames = align.process(frames);
        auto color_frame = frames.get_color_frame();
        auto depth_frame = frames.get_depth_frame();

        /***** PROCESS FRAMES *****/
        auto depth = frames
                .apply_filter(temporal);

        
        /**** DEPTH FRAME ****/
        //m_work_matrix = depth_frame_to_meters(frames.get_depth_frame());

        cv::Mat color_mat(
                cv::Size(color_frame.get_width(), color_frame.get_height()),
                CV_8UC3,
                (void *) color_frame.get_data(),
                cv::Mat::AUTO_STEP
            );
        m_work_matrix = color_mat.clone();

        clog(info) << get_id() <<" provided Matrix!" << std::endl;
    };
};

#endif //DUNE_PIPELINE_CAMERAPROVIDER_HPP
