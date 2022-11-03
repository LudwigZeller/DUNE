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
| @file main.cpp                         |
| @author David Schoosleitner            |
| @date Nov. 3rd 2022                    |
|                                        |
| This file is responsible for:          |
|  - Default Manager class               |
| This file depends on:                  |
|  -                                     |
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
*****************************************/


/*****************************************
|               INCLUDES                 |
*****************************************/
#include "DManager.hpp"
#include <iostream>
#include <string>


/*****************************************
|          GLOBAL DEFINITIONS            |
*****************************************/

/**
 * @fn DManager::DManager::DManager(const struct DManagerFlags &&d)
 * @brief Construct a new DManager object
 * @param d Flags to control Manager parameters
 */
DManager::DManager::DManager(const struct DManagerFlags &&d):
    running(0),
    thread([this] () -> void { this->run(); }),
    terminated(0),
    milli_last_ela(0),
    canrun(0),
    flags(d),
    schedule(std::chrono::steady_clock::now() + d.delay_target_period)
{
}

/**
 * @fn DManager::DManager::~DManager()
 * @brief Termination is called upon destruction. Thread and mutex are "freed"
 * @ref https://stackoverflow.com/questions/52610776/how-to-wake-a-stdthread-while-it-is-sleeping
 */
DManager::DManager::~DManager()
{
    //! Takes ownership over mutex in scope. Mutex "freed" upon destruction
    {
        std::lock_guard<std::mutex> l(this->mutex);
    }

    //!! Terminates the thread if not already
    if(this->terminated == 0)
    {
        this->terminated = 1;
        this->cv.notify_one();
        this->thread.join();
        std::cout << "Automatic termination of DManager " << std::endl;
    }
}

/**
 * @fn void DManager::DManager::run()
 * @brief this code will be run in a seperate thread
 */
void DManager::DManager::run()
{
    //! Timing init
    auto start = std::chrono::steady_clock::now();
    auto ela = start-start;

    //! User defined init (in scope)
    this->init_user();

    //! Flag as available and wait
    this->canrun = 1;
    this->wait_for();

    //!! Thread loops
    while(!this->is_closed())
    {
        //! Start time
        start = std::chrono::steady_clock::now();

        //! Run User code
        this->run_user();

        //! Elaborate time
        this->milli_last_ela = 
            std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now() - start)
            .count();
        
        //! Print ela
        if(this->flags.log_ela)
        {
            std::cout << "Thread " << std::this_thread::get_id() << ", \"" << this->flags.name << "\" took " << this->milli_last_ela << "ms." << std::endl;
        }

        //! Wait until next notify
        this->wait_for();
    }
}

/**
 * @fn bool DManager::DManager::wait_for()
 * @brief Wait for is called by DManager::run
 * 
 * @return true = thread while loop continues ||
 * @return false = thread while loop breaks
 */
bool DManager::DManager::wait_for()
{
    //! Do not run when terminated
    if(this->terminated > 0)
        return false;

    //! Skip a waiting sequence, that means, catch up if lagging
    if(--this->running > 0)
        return true;
    
    //!! @ref https://stackoverflow.com/questions/52610776/how-to-wake-a-stdthread-while-it-is-sleeping
    //!! Waiting for 10 seconds or until notified
    std::unique_lock<std::mutex> l(this->mutex);
    return !this->cv.wait_for(
        l,
        std::chrono::seconds(10),
        [this] () -> bool { return this->is_closed() || this->is_running(); }
    );
}

/**
 * @fn void DManager::DManager::terminate()
 * @brief Manual termination of thread, mutex is not freed
 */
void DManager::DManager::terminate()
{
    //!! Terminate thread, do not interfere with mutex outside of deconstructor
    this->terminated = 1;
    this->cv.notify_one();
    this->thread.join();
    std::cout << "Manual termination of DManager " << std::endl;
}

/**
 * @fn void DManager::DManager::notify()
 * @brief Called by DManager::DControlManager, wakes up from waiting
 */
void DManager::DManager::notify()
{
    if(this->flags.can_skip)
        this->running = 1;
    else
        this->running++;
    this->cv.notify_one();
}
