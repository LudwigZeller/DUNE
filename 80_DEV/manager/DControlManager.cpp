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


/*****************************************
|               INCLUDES                 |
*****************************************/
#include "DControlManager.hpp"
#include <iostream>
#include <chrono>


/*****************************************
|          GLOBAL DEFINITIONS            |
*****************************************/

/**
 * @fn DManager::DControlManager::~DControlManager()
 * @brief Destroy the DControlManager object
 */
DManager::DControlManager::~DControlManager()
{
    // @deprecated:
    //call_termination();
}

/**
 * @fn void DManager::DControlManager::call_termination()
 * @brief Invokes termination of all manager members
 * @deprecated Not required as managers are automatically terminated and their threads freed upon destruction.
 */
void DManager::DControlManager::call_termination()
{
  this->dnm.terminate();
}

/**
 * @fn int DManager::DControlManager::launch()
 * @brief Loops and notifies managers according to their scedule, launch in Main thread!
 * @return int exit code 
 */
int DManager::DControlManager::launch()
{
    using namespace std::chrono;

    //!! Wait until all members finishes initialization
    while(!dnm.can_run());

    //! Temporary start condition
    auto start = steady_clock::now();

    while(duration_cast<seconds>(steady_clock::now()-start).count() < 10)
    {
        if(dnm.in_schedule())
        {
            dnm.notify();
        }
    }   

    return 0;
}
