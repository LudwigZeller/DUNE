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
#include "utils.hpp"
#include "Window.hpp"

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

/*****           MISC               *****/
#include <iostream>
#include <chrono>

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */

int main(int argc, char **argv)
{
    std::cout << "Starting DepthCamera, running in " << std::this_thread::get_id() << std::endl;
    CameraProvider camera_provider{"Camera_Provider"};
    //Test_Provider camera_provider{"Test_Provider"};

    Window window{"DUNE", FULLSCREEN, "DP-4"};
    WindowWorker window_worker{"Window_Worker", &window};
    Filter::TemporalWorker temporal_worker1{"Filter_Temporal_Worker1"};
    Filter::ColorizeWorker colorize_worker{"Filter_Colorize_Worker"};
    Filter::LineWorker line_worker{"Filter_Line_Worker"};
    Filter::InterpolatorWorker interpolator_worker{"Filter_Interpolator_Worker"};
    Filter::DiscreticiserWorker discreticiser_worker{"Filter_Discreticiser_Worker"};

    Pipeline pipeline{&camera_provider};

    pipeline.push_worker(&discreticiser_worker);
    pipeline.push_worker(&temporal_worker1);
    pipeline.push_worker(&colorize_worker);
    pipeline.push_worker(&line_worker);
    pipeline.push_worker(&interpolator_worker);
    pipeline.push_worker(&window_worker);
    
    pipeline.start();

    while(window);
}

