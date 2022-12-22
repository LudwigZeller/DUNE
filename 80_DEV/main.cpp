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

/*****           MISC               *****/
#include <iostream>
#include <chrono>

#include <regex>

cv::Point2i translation_vec = {120,50};
cv::Size scalar_kernel = {(int)(0.85 * STREAM_WIDTH), (int)(0.85 * STREAM_HEIGHT)};

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */

int main(int argc, char **argv)
{
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
            scalar_kernel.width = (int)(std::atof(smat[4].str().c_str()) * STREAM_HEIGHT);
        }
    }


    std::srand((unsigned)std::time(nullptr));
    std::cout << "Starting DepthCamera, running in " << std::this_thread::get_id() << std::endl;
    CameraProvider camera_provider{"Camera_Provider"};
    //Test_Provider camera_provider{"Test_Provider"};

    Window window{"DUNE", FULLSCREEN, true};
    WindowWorker window_worker{"Window_Worker", &window};
    Filter::TemporalWorker temporal_worker{"Filter_Temporal_Worker"};
    Filter::ColorizeWorker colorize_worker{"Filter_Colorize_Worker"};
    Filter::LineWorker line_worker{"Filter_Line_Worker"};
    Filter::InterpolatorWorker interpolator_worker{"Filter_Interpolator_Worker"};
    Filter::DiscreticiserWorker discreticiser_worker{"Filter_Discreticiser_Worker"};
    Filter::ScaleWorker scale_worker{"Filter_Scale_Worker"};
    Filter::FunnyWorker fw{"fw"};
    Filter::checker ch{"ch"};
    CalibWorker calib_worker{"Calib_Worker"};
    TranslatorWorker translator_worker{"Translator_Worker", true};

    Pipeline pipeline{&camera_provider};
    Pipeline pipeline2{&camera_provider};
    Pipeline pipelinec{&camera_provider};

    pipeline.push_worker(&discreticiser_worker);
    pipeline.push_worker(&scale_worker);
    pipeline.push_worker(&translator_worker);
    pipeline.push_worker(&temporal_worker);
    pipeline.push_worker(&ch);
    pipeline.push_worker(&line_worker);
    pipeline.push_worker(&colorize_worker);
    //pipeline.push_worker(&interpolator_worker);
    pipeline.push_worker(&fw);
    pipeline.push_worker(&window_worker);

    pipeline2.push_worker(&discreticiser_worker);
    pipeline2.push_worker(&scale_worker);
    pipeline2.push_worker(&translator_worker);
    pipeline2.push_worker(&temporal_worker);
    pipeline2.push_worker(&colorize_worker);
    pipeline2.push_worker(&interpolator_worker);
    pipeline2.push_worker(&window_worker);

    pipelinec.push_worker(&discreticiser_worker);
    pipelinec.push_worker(&temporal_worker);
    //pipelinec.push_worker(&translator_worker);
    pipelinec.push_worker(&calib_worker);
    //pipelinec.push_worker(&colorize_worker);
    pipelinec.push_worker(&window_worker);

    //test(window);
    
    pipelinec.start();

    int state = 0;

    while(window)
    {
        if(window.wpressed)
        {
            window.wpressed = false;
            if(state == 2)
            {
                pipeline2.stop();
                pipeline.start();
                state = 1;
            }
            else if(state == 0)
            {
                pipelinec.stop();
                pipeline.start();
                state = 1;
            }
            else if(state == 1)
            {
                pipeline.stop();
                pipeline2.start();
                state = 2;
            }
        }
    }
}

