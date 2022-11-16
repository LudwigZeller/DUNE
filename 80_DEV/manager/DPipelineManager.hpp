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
        if(RTE::camera_connected && !RTE::window.getshowcap())
        {
            //! Switch upon 'c' keypress
            if(RTE::window.getssw())
            {
                sw = !sw;
                RTE::window.resssw();
            }

            //! GL Viewport
            glViewport(RTE::resizex, RTE::resizey, RTE::resizewidth, RTE::resizeheight);
            //glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
            //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

            //!! Lock
            RTE::write_matrix_mutex.lock();
            RTE::depth_matrix_mutex.lock();

            //const cv::Mat m

            // Constants
            const double line = 0.001;
            const cv::Vec3b black{0,0,0};
            const cv::Vec3b blue{255,0,0};
            const cv::Vec3b green{33, 176, 50};
            const cv::Vec3b brown{27, 76, 105};
            const cv::Vec3b lightbrown{39, 159, 186};
            const cv::Vec3b red{50, 50, 179};
            const cv::Vec3b white{255,255,255};
            const cv::Vec3b grey{196,196,196};

            if(!RTE::write_matrix.empty())
            {
                if(RTE::write_history3.empty())
                {
                    RTE::write_history3 = RTE::depth_matrix;
                    RTE::write_history2 = RTE::depth_matrix;
                    RTE::write_history1 = RTE::depth_matrix;
                }

                RTE::write_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    double depth = RTE::depth_matrix.at<double>(pos)
                        +   RTE::write_history1.at<double>(pos)
                        +   RTE::write_history2.at<double>(pos)
                        +   RTE::write_history3.at<double>(pos);
                    depth /= 4.0;

                    if(depth < 1)
                        pixel = black;
                    else if(depth > 1 && depth < (1.05 - line)){
                        pixel = white;
                    }
                    else if(depth > (1.05 + line) && depth < (1.1 - line)){
                        pixel = red;
                    }
                    else if(depth > (1.1 + line) && depth < (1.13 - line)){
                        pixel = lightbrown;
                    }
                    else if(depth > (1.13 + line) && depth < (1.18 - line)){
                        pixel = green;
                    }
                    else if(depth > (1.18 + line) && depth < 1.24){
                        pixel = blue;
                    }
                    else if(depth > 1.24){
                        pixel = black;
                    }
                    else{
                        pixel = grey;
                    }
                });

                RTE::write_history3 = RTE::write_history2;
                RTE::write_history2 = RTE::write_history1;
                RTE::write_history1 = RTE::depth_matrix;
            }
            
            //! Draw from matrix
            if(sw) {
                if(RTE::window.getcapture())
                {
                    capture(RTE::color_matrix);
                    captureDepth(RTE::xtra);
                    RTE::window.rescapture();
                }
                draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), RTE::color_matrix);
            }
            else {
                if(RTE::window.getcapture())
                {
                    capture(RTE::write_matrix);
                    captureDepth(RTE::xtra);
                    RTE::window.rescapture();
                }
                draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), RTE::write_matrix);
            }
            RTE::write_matrix_mutex.unlock();
            RTE::depth_matrix_mutex.unlock();
        }
        else if(RTE::window.getshowcap())
        {
            #include "../../90_BUILD/capture-0.dres"
            #include "../../90_BUILD/capture-1.dres"
            #include "../../90_BUILD/capture-2.dres"
            #include "../../90_BUILD/capture-3.dres"
            #include "../../90_BUILD/capture-4.dres"
            #include "../../90_BUILD/capture-5.dres"
            #include "../../90_BUILD/capture-6.dres"
            #include "../../90_BUILD/capture-7.dres"
            #include "../../90_BUILD/capture-8.dres"
            #include "../../90_BUILD/capture-9.dres"
            #include "../../90_BUILD/capture-10.dres"
            #include "../../90_BUILD/capture-11.dres"
            #include "../../90_BUILD/capture-12.dres"
            #include "../../90_BUILD/capture-13.dres"
            #include "../../90_BUILD/capture-14.dres"
            #include "../../90_BUILD/capture-15.dres"
            #include "../../90_BUILD/capture-16.dres"

            static int s_i = RTE::window.getisels();
            #define atm(a) assetToMat(CAPTURE_ ## a ## _WIDTH, CAPTURE_ ## a ## _HEIGHT, CAPTURE_ ## a ## _DATA);
            static cv::Mat s_mat = atm(0);

            if(s_i != RTE::window.getisels())
            {
                s_i = RTE::window.getisels();

                switch(s_i)
                {
                    case 0:
                        s_mat = atm(0);
                    break;
                    case 1:
                        s_mat = atm(1);
                    break;
                    case 2:
                        s_mat = atm(2);
                    break;
                    case 3:
                        s_mat = atm(3);
                    break;
                    case 4:
                        s_mat = atm(4);
                    break;
                    case 5:
                        s_mat = atm(5);
                    break;
                    case 6:
                        s_mat = atm(6);
                    break;
                    case 7:
                        s_mat = atm(7);
                    break;
                    case 8:
                        s_mat = atm(8);
                    break;
                    case 9:
                        s_mat = atm(9);
                    break;
                    case 10:
                        s_mat = atm(10);
                    break;
                    case 11:
                        s_mat = atm(11);
                    break;
                    case 12:
                        s_mat = atm(12);
                    break;
                    case 13:
                        s_mat = atm(13);
                    break;
                    case 14:
                        s_mat = atm(14);
                    break;
                    case 15:
                        s_mat = atm(15);
                    break;
                    default:
                        s_mat = atm(16);
                }
            }
            
            glViewport(RTE::resizex, RTE::resizey, RTE::resizewidth, RTE::resizeheight);
            draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), s_mat);
        }
        //! Or draw idle logo
        else
        {
            fun();
        }

        //!! Compiler-level configurable on-screen debug information
        #if DEBUG_ON_SCREEN
        debug_str = "Last ela:\n " + std::to_string(RTE::millis_pm) + "ms[PL]\n " + std::to_string(RTE::millis_rm) + "ms[RS]";
        draw_text_debug(10, 50, debug_str.c_str());
        #endif
    }

    /**
     * @fn inline void fun()
     * @brief Idler function, draws logo on window
     */
    inline void fun()
    {
        #define MAXV 20

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
