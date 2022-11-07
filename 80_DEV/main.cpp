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
#include "manager/DControlManager.hpp"

/*****           MISC               *****/
#include <iostream>

typedef cv::Vec3b Pixel;
/****************************************\
|               MAIN FN                  |
\****************************************/

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */

int main(int argc, char **argv)
{
    //!! Parse arguments
    char *device_name = nullptr;        //< Display device name

    for(int i = 0; i < argc; ++i)
    {
        if((strlen(argv[i]) > 2) && (argv[i][0] == '-') && (argv[i][1] == 'm'))
        {
            device_name = argv[i] + 2;
        }
    }
    
    //!! Wait for init and run controller
    while(!RTE::window.initialized());
    DManager::DControlManager control;
    control.launch();

    if(RTE::window)
    {
        RTE::window.close();
        while(RTE::window);
    }

    clog(info) << "Program exited normaly" << std::endl;
    return RTE::exit_code_pipeline;
}

/*
depth_matrix.forEach<Pixel>([raw_matrix](Pixel &p, const int *pos){
            const int x = pos[1];
            const int y = pos[0];
            const cv::float16_t &raw = raw_matrix.at<cv::float16_t>(y, x);
            //cv::Vec3b &col_dat = dm->at<cv::Vec3b>(pos[0],pos[1]);
            //const float dep = depth_frame.get_distance(pos[0], pos[1]);
            //col_dat = (vec_v5-vec_ve*dep_dat);

            p = {
                (unsigned char)(3900000.0f*(1 - raw)),(unsigned char)(3900000.0f*(1 - raw)),
                (unsigned char)(3900000.0f*(1 - raw))
            };
        });
*/
