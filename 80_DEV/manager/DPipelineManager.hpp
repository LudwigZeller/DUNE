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
| @file DPipelineManager.cpp             |
| @author David Schoosleitner            |
| @date Nov. 7th 2022                    |
|                                        |
| This file is responsible for:          |
|  - Controling window and OpenGL-Context|
| This file depends on:                  |
|  - Managers                            |
|  - RTE                                 |
|  - Local utilities                     |
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
#include "DManager.hpp"
#include "../config.hpp"
#include "../utils.hpp"
#include "../RTE.hpp"
#include "../assets.dres"

using namespace std::chrono;


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
 * CLASS DPipelineManager : public DManager
 **********************
 * @class DPipelineManager : public DManager
 * @brief Responsible for controling window contents and OpenGL context of the main window.
 * @brief It takes over Main thread context and draws from data stored on RTE. [1/FPS]
 **********************/
class DPipelineManager : public DManager
{
public:

    /**
     * @fn DPipelineManager()
     * @brief Construct a new DPipelineManager object.
     * @brief DManagerFlags are set by default.
     */
    DPipelineManager() : DManager(DManagerFlags{
        .can_skip = false,                                  //< Graphics are not allowed to skip and will catch up with schedule
        .log_lag = true,                                    //< Window drawing should not lag unless there is an issue
        .target_period = milliseconds(int(1000.0f / FPS)),  //< FPS controls schedule
        .delay_target_period = milliseconds(0),             //< No delay
        .log_ela = false,                                   //< No elapsed time print
        .name = "Pipeline Manager",                         //< Name
        .adress_millis = &RTE::millis_pm                    //< Address to store passed time into
    })
    {
    }

    /*****       CLASS MEMBERS       *****/

    //! Color/Depth stream switch
    bool sw = false;

    //! DUNE logo, loaded on init
    cv::Mat logo;

    //! Tick counter
    unsigned long long tick;

    #if DEBUG_ON_SCREEN
    //! DEBUG: Debug string will be drawn to the top-left of the window if active.
    std::string debug_str;
    #endif

    /*****       CLASS FUNCTIONS       *****/

    /**
     * @fn virtual void init_user() override
     * @brief Overridden initialization of Pipeline Manager.
     * @brief Takes over context and loads members and window.
     */
    virtual void init_user() override
    {
        clog(info) << "Starting Pipeline manager" << std::endl;

        //! Wait for window to be initialized
        while(!RTE::window.initialized());

        //! Take over context
        glfwMakeContextCurrent(RTE::window.getwndptr());

        //!! Member init
        logo = assetToMat(DLOGO_WIDTH, DLOGO_HEIGHT, DLOGO_DATA);
        //cv::imread("DuneLogo.png");
        debug_str = "";
        tick = 0ULL;

        //!! Window validity and set RTE bool
        while(!RTE::window);
        RTE::window_open = true;
    }

    /**
     * @fn virtual void run_user() override
     * @brief Cyclic routine of Pipeline Manager.
     * @brief Takes RTE frames and displays them. Triggers Manager termination upon window closing.
     */
    virtual void run_user() override
    {
        tick++;

        //!! All routines are to be terminated upon window close
        if(!RTE::window)
        {
            RTE::window_open = false;
            RTE::exit_code_pipeline = 0;
            RTE::terminate_all = true;
            return;
        }

        //! Draw drame
        if(RTE::camera_connected)
        {
            //! Switch upon 'c' keypress
            if(RTE::window.getssw())
            {
                sw = !sw;
                RTE::window.resssw();
            }

            //! Draw from matrix
            if(sw)
                draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), RTE::color_matrix);
            else
                draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), RTE::window_out_matrix);
        }
        //! Or draw idle logo
        else
        {
            //draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), logo);
            fun();
        }

        //!! Compiler-level configurable on-screen debug information
        #if DEBUG_ON_SCREEN
        debug_str = "Last ela:\n " + std::to_string(RTE::millis_pm) + "ms[PL]\n " + std::to_string(RTE::millis_rm) + "ms[RS]";
        draw_text_debug(10, 10, debug_str.c_str());
        #endif
    }

    /**
     * @fn inline void fun()
     * @brief Idler function, draws logo on window
     */
    inline void fun()
    {
        #define MAXV 10

        static int x = 0;
        static int y = 0;
        static int vx = MAXV;
        static int vy = MAXV;
        int iw = (int)((float) RTE::window.realwidth() * .2f);
        int ih = (int)((float) RTE::window.realheight() * .2f);
        iw = ih = (iw > ih) ? ih : iw;
        const int w = RTE::window.realwidth() - iw;
        const int h = RTE::window.realheight() - ih;

        //!! Move to coord
        x += vx;
        y += vy;

        //!! Check collision and push + reflect
        //! North and south
        if(y < 0 || y > h)
        {
            vy = -vy;
            while(y < 0 || y > h)
                y += vy;
        }
        //! West and east
        if(x < 0 || x > w)
        {
            vx = -vx;
            while(x < 0 || x > w)
                x += vx;
        }

        //! Draw
        draw_frame(cv::Size((int) iw, (int) ih),
            logo * (1.0f - (0.5f * cos((1.234f * (float) tick) / FPS))),
            cv::Size((int) x, (int) y)
        );
    }
};

};
