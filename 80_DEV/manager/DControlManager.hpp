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
| @file DControlManager.cpp              |
| @author David Schoosleitner            |
| @date Oct. 26th 2022                   |
|                                        |
| This file is responsible for:          |
|  - Controling timings of multiple      |
|    application sections and threads    |
| This file depends on:                  |
|  - Managers                            |
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
#include "DPipelineManager.hpp"
#include "DrsGrabberManager.hpp"
#include <iostream>
#include <chrono>
#include "../RTE.hpp"


/*****************************************
|          GLOBAL DECLARATIONS           |
*****************************************/

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
 * CLASS DControlManager
 **********************
 * @class DControlManager
 * @brief Responsible for timings and execution of individual
 * @brief Managers and Threads. To be launched in main thread!
 **********************/
class DControlManager
{
private:

    //!! Managers are stored as members. Define and override their user functions via included .hpp
    DPipelineManager pipelineMgr{};
    //DrsGrabberManager grabberMgr{};

public:

    /**
     * @fn DManager::DControlManager::~DControlManager()
     * @brief Destroy the DControlManager object
     */
    ~DControlManager()
    {
        //call_termination();
    }

    /**
     * @fn void DManager::DControlManager::call_termination()
     * @brief Invokes termination of all manager members
     * @deprecated Not required as managers are automatically terminated and their threads freed upon destruction.
     */
    void call_termination()
    {
        this->pipelineMgr.terminate();
        //this->grabberMgr.terminate();
    }

    /**
     * @fn void DManager::DControlManager::launch()
     * @brief Loops and notifies managers according to their scedule, launch in Main thread!
     */
    void launch()
    {
        //long LOL = 100000000ULL;
        using namespace std::chrono;

        //!! Wait until all members finish initialization
        //while(!grabberMgr.can_run());
        while(!pipelineMgr.can_run());

        clog(info) << "DONE! Loop starting!" << std::endl;
    
        //!! Main loop
        while(!RTE::terminate_all)
        {
            //if(grabberMgr)
            //    grabberMgr.notify();
                
            if(pipelineMgr)
                pipelineMgr.notify();

            //if(--LOL <= 0)
            //{
            //    if (!glfwWindowShouldClose(RTE::window.getwndptr()))
            //clog(info) << "Window Force Escape" << std::endl;
        //glfwSetWindowShouldClose(RTE::window.getwndptr(), GLFW_TRUE);
            //}
        }

        //!! Extra ending functions
        //this->grabberMgr.end_stream();
        //this->call_termination();
    }
};

};