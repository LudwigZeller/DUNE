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

    // Constants
    const double line = 0.001;
    const cv::Vec3b black{0,0,0};
    const cv::Vec3b blue{255,0,0};
    const cv::Vec3b dark_blue{150,0,0};
    const cv::Vec3b green{33, 176, 50};
    const cv::Vec3b brown{27, 76, 105};
    const cv::Vec3b yellow{39, 159, 186};
    const cv::Vec3b red{50, 50, 179};
    const cv::Vec3b white{255,255,255};
    const cv::Vec3b grey{196,196,196};

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

    cv::Mat raw;

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
        #include "../../90_BUILD/test1.dres"
        macro_depthAssetToMatrix(raw, CAPTURE_DEPTH_1_WIDTH, CAPTURE_DEPTH_1_HEIGHT, CAPTURE_DEPTH_1_DATA);
        macro_matrixToMeters(raw);

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

            //! GL Viewport
            glViewport(RTE::resizex, RTE::resizey, RTE::resizewidth, RTE::resizeheight);
            //glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
            //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

            //!! Lock
            RTE::write_matrix_mutex.lock();
            RTE::depth_matrix_mutex.lock();

            //const cv::Mat m

            if(!RTE::write_matrix.empty())
            {
                if(RTE::write_history3.empty())
                {
                    RTE::write_history3 = RTE::depth_matrix;
                    RTE::write_history2 = RTE::depth_matrix;
                    RTE::write_history1 = RTE::depth_matrix;
                }

                /// @name Original Depth brute force
                /*RTE::write_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
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
                });*/

                /// @name Gradient
                /*RTE::write_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    double depth = RTE::depth_matrix.at<double>(pos)
                        +   RTE::write_history1.at<double>(pos)
                        +   RTE::write_history2.at<double>(pos)
                        +   RTE::write_history3.at<double>(pos);
                    depth /= 4.0;

                    if(depth < 1 || depth > 1.24)
                    {
                        pixel = black;
                    }
                    else
                    {
                        float grad = (depth - 1.0) * 10 + tick / 10.0;

                        pixel = red * sin(grad) + blue * sin(grad + 2*3.14159/3.0) + green * sin(grad + 4*3.14159/3.0);
                    }
                });*/

                cv::Mat wm = RTE::write_matrix.clone();

                wm.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    double depth = RTE::depth_matrix.at<double>(pos)
                        +   RTE::write_history1.at<double>(pos)
                        +   RTE::write_history2.at<double>(pos)
                        +   RTE::write_history3.at<double>(pos);
                    depth /= 4.0;

                    if(depth < 1 || depth > 1.24)
                    {
                        pixel = black;
                    }
                    else
                    {
                        int grad = ((depth - 1.0) / 0.24) * 6;

                        pixel =
                            (0 == grad) ? white :
                            (1 == grad) ? red   :
                            (2 == grad) ? yellow:
                            (3 == grad) ? green :
                            (4 == grad) ? blue  :
                            dark_blue;
                    }
                });

                wm.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    
                });

                RTE::write_matrix = wm;

                /*RTE::write_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    cv::Vec3b eq = wm.at<cv::Vec3b>(pos);

                    for(int i = 1; i < 9; i++)
                    {
                        int y = pos[0] - 1 + i / 3;
                        int x = pos[1] + 1 + i % 3;
                        if(y > 0 && y < RTE::write_matrix.size().height &&
                            x > 0 && x < RTE::write_matrix.size().width)
                        if(eq != wm.at<cv::Vec3b>(y, x))
                        {
                            pixel = grey;
                            return;
                        }
                    }

                    pixel = eq;
                });*/

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
        //! Or draw idle logo
        else
        {
            cv::Mat wm(raw.size(), CV_8UC3);

                wm.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    double depth = raw.at<double>(pos);

                    if(depth < 1 || depth > 1.24)
                    {
                        pixel = black;
                    }
                    else
                    {
                        int grad = ((depth - 1.0) / 0.24) * 6;

                        pixel =
                            (0 == grad) ? white :
                            (1 == grad) ? red   :
                            (2 == grad) ? yellow:
                            (3 == grad) ? green :
                            (4 == grad) ? blue  :
                            dark_blue;
                    }
                });

                
                cv::Vec3b *wm_dat = wm.ptr<cv::Vec3b>();
                cv::Size wm_s = wm.size();
                cv::Mat wm2(wm_s, CV_8UC3);

                wm2.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos)
                {
                    cv::Vec3b eq = wm.at<cv::Vec3b>(pos);

                    for(int i = 1; i < 9; i++)
                    {
                        int y = pos[0] - 1 + i / 3;
                        int x = pos[1] + 1 + i % 3;
                        if(y > 0 && y < wm_s.height &&
                            x > 0 && x < wm_s.width)
                        if(eq != wm_dat[x + y * wm_s.width])
                        {
                            pixel = grey;
                            return;
                        }
                    }

                    pixel = eq;
                });
            
            RTE::resizewidth = RTE::window.realheight() * (float) raw.size().width / (float) raw.size().height;
            RTE::resizeheight = RTE::window.realheight();
            RTE::resizex = 0.0f;
            RTE::resizey = abs(RTE::resizeheight - RTE::window.realheight()) / 2;

            glViewport(RTE::resizex, RTE::resizey, RTE::resizewidth, RTE::resizeheight);
            draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), wm2);
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
