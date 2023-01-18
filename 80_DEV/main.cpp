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
#include <atomic>

/*****        LOCAL INCLUDE         *****/
#include "config.hpp"
#include "utils.hpp"
#include "Window.hpp"
#include "Calibration.hpp"

// Worker
#include "Pipeline/Pipeline.hpp"
#include "Pipeline/WindowWorker.hpp"
#include "Pipeline/CameraProvider.hpp"
//#include "Pipeline/Test_Provider.hpp"
#include "Pipeline/FilterColorizeWorker.hpp"
#include "Pipeline/FilterTemporalWorker.hpp"
#include "Pipeline/FilterLineWorker.hpp"
#include "Pipeline/FilterInterpolatorWorker.hpp"
#include "Pipeline/FilterDiscreticiser.hpp"
#include "Pipeline/FilterScaleWorker.hpp"
#include "Pipeline/FunnyW.hpp"
#include "Pipeline/checker.hpp"
#include "Pipeline/Calib.hpp"
#include "Pipeline/Translator.hpp"
#include "Pipeline/FilterDifferenceWorker.hpp"
#include "TouchwndRes.hpp"
#include "Pipeline/FilterStripeWorker.hpp"
#include "Pipeline/VisualCutWorker.hpp"

/*****           MISC               *****/
#include <iostream>
#include <chrono>

#include <regex>

cv::Point2i translation_vec = {-16,17};
cv::Size scalar_kernel = {(int)(STREAM_WIDTH), (int)(0.993 * STREAM_HEIGHT)};
volatile bool stay_in_calib = true;

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */

int main(int argc, char **argv)
{
    initMats();
    std::regex _mog{"-q([-0-9]+),([-0-9]+),([0-9.]+),([0-9.]+)"};
    std::smatch smat;

    for(int i = 1; i < argc; i++)
    {
        std::string l{argv[i]};
        if(std::regex_search(l, smat, _mog))
        {
            translation_vec.x = std::atoi(smat[1].str().c_str());
            translation_vec.y = std::atoi(smat[2].str().c_str());
            scalar_kernel.width = (int)(std::atof(smat[3].str().c_str()) * STREAM_WIDTH);
            scalar_kernel.height = (int)(std::atof(smat[4].str().c_str()) * STREAM_HEIGHT);

            clog(warn) << "Value take-over: \ntv.x = " << translation_vec.x << "\ntv.y = " << translation_vec.y << "\nsk.w = " << scalar_kernel.width << "\nsk.h = " << scalar_kernel.height << std::endl;
        }
    }


    std::srand((unsigned)std::time(nullptr));
    std::cout << "Starting DepthCamera, running in " << std::this_thread::get_id() << std::endl;
    CameraProvider camera_provider{"Camera_Provider"};
    //Test_Provider camera_provider{"Test_Provider"};

    Window window{"DUNE", MONITOR_BEAMER_FIX};
    Window twindow{"DUNE TOUCH", MONITOR_TOUCHDISPLAY_FIX};
    //Window 
    WindowWorker window_worker{"Window_Worker", &window};
    Filter::TemporalWorker temporal_worker{"Filter_Temporal_Worker"};
    Filter::ColorizeWorker colorize_worker{"Filter_Colorize_Worker", Filter::ColorizeWorker::COLORIZE_TYPE_e::DEFAULT};
    Filter::ColorizeWorker rgbg_colorize_worker{"Filter_Colorize_RGBG_Worker", Filter::ColorizeWorker::COLORIZE_TYPE_e::RGBG};
    Filter::LineWorker line_worker{"Filter_Line_Worker"};
    Filter::InterpolatorWorker interpolator_worker{"Filter_Interpolator_Worker"};
    Filter::DiscreticiserWorker discreticiser_worker{"Filter_Discreticiser_Worker"};
    Filter::ScaleWorker scale_worker{"Filter_Scale_Worker"};
    Filter::FunnyWorker fw{"fw"};
    Filter::checker ch{"ch"};
    Filter::DifferenceWorker difference_worker{"Filter_Difference_Worker"};
    Filter::StripeWorker stripe_worker{"Filter_Stripe_Worker"};
    CalibWorker calib_worker{"Calib_Worker"};
    TranslatorWorker translator_worker{"Translator_Worker", true};
    Filter::VisualCutWorker visual_cut_worker{"Visual_Cut_Worker"};

    Pipeline pipeline_minecraft{&camera_provider};
    Pipeline pipeline_smooth{&camera_provider};
    Pipeline pipeline_difference{&camera_provider};
    Pipeline pipeline_stripe{&camera_provider};
    Pipeline pipeline_calibration{&camera_provider};

    pipeline_minecraft.push_worker(&discreticiser_worker);
    pipeline_minecraft.push_worker(&scale_worker);
    pipeline_minecraft.push_worker(&translator_worker);
    pipeline_minecraft.push_worker(&visual_cut_worker);
    pipeline_minecraft.push_worker(&temporal_worker);
    pipeline_minecraft.push_worker(&ch);
    pipeline_minecraft.push_worker(&line_worker);
    pipeline_minecraft.push_worker(&colorize_worker);
    pipeline_minecraft.push_worker(&fw);
    pipeline_minecraft.push_worker(&window_worker);

    pipeline_smooth.push_worker(&discreticiser_worker);
    pipeline_smooth.push_worker(&scale_worker);
    pipeline_smooth.push_worker(&translator_worker);
    pipeline_smooth.push_worker(&temporal_worker);
    pipeline_smooth.push_worker(&rgbg_colorize_worker);
    pipeline_smooth.push_worker(&interpolator_worker);
    pipeline_smooth.push_worker(&window_worker);

    pipeline_difference.push_worker(&discreticiser_worker);
    pipeline_difference.push_worker(&scale_worker);
    pipeline_difference.push_worker(&translator_worker);
    pipeline_difference.push_worker(&temporal_worker);
    pipeline_difference.push_worker(&difference_worker);
    pipeline_difference.push_worker(&colorize_worker);
    pipeline_difference.push_worker(&interpolator_worker);
    pipeline_difference.push_worker(&window_worker);

    pipeline_stripe.push_worker(&discreticiser_worker);
    pipeline_stripe.push_worker(&scale_worker);
    pipeline_stripe.push_worker(&translator_worker);
    pipeline_stripe.push_worker(&temporal_worker);
    pipeline_stripe.push_worker(&stripe_worker);
    pipeline_stripe.push_worker(&colorize_worker);
    pipeline_stripe.push_worker(&interpolator_worker);
    pipeline_stripe.push_worker(&window_worker);

    pipeline_calibration.push_worker(&discreticiser_worker);
    pipeline_calibration.push_worker(&calib_worker);
    pipeline_calibration.push_worker(&window_worker);

#define __DO_CALIB
#ifdef __DO_CALIB
    pipeline_calibration.start();
    while(stay_in_calib && window && twindow);
    pipeline_calibration.stop();
#endif

    pipeline_minecraft.start();

    int state = twindow.get_dat();
    twindow.render_matrix(getCaptureIndex(state));

    while(window && twindow)
    {
        if(state != twindow.get_dat())
        {
            twindow.cooldown = true;
            state = twindow.get_dat();
            twindow.render_matrix(getCaptureIndex(state));
            twindow.operator bool();

            pipeline_minecraft.stop();
            pipeline_smooth.stop();
            pipeline_difference.stop();
            pipeline_stripe.stop();

            if(state == 0)
            {
                pipeline_minecraft.start();
            }
            else if(state == 1)
            {
                pipeline_smooth.start();
            }
            else if(state == 2)
            {
                pipeline_difference.start();
            }
            else if(state == 3)
            {
                pipeline_stripe.start();
            }

            difference_worker.reset_save();
            twindow.cooldown = false;
        }
        if(twindow.docapture)
        {
            twindow.docapture = false;
            cv::Mat mat = window.capture();
            saveMatAsFile(mat, "CAPTURE", "./capture.dres");
        }
    }

    window.close();
    twindow.close();
    while(window || twindow);
}
