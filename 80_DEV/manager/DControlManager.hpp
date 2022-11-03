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
#include "DNewManager.hpp"


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
    DNewManager dnm;

public:

    /**
     * @fn DManager::DControlManager::~DControlManager()
     * @brief Destroy the DControlManager object
     */
    ~DControlManager();

    /**
     * @fn void DManager::DControlManager::call_termination()
     * @brief Invokes termination of all manager members
     * @deprecated Not required as managers are automatically terminated and their threads freed upon destruction.
     */
    void call_termination();

    /**
     * @fn int DManager::DControlManager::launch()
     * @brief Loops and notifies managers according to their scedule, launch in Main thread!
     * @return int exit code 
     */
    int launch();
};

};
