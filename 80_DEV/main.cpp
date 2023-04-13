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
#include <glad/glad.h>          //<  OpenGL function loader; Important: Before GLFW
#include <GLFW/glfw3.h>         //<  OpenGL library for window and input
#include <librealsense2/rs.hpp> //<  Intel(R) RealSenseTM 3D-Camera SDK
#include <opencv2/opencv.hpp>   //!< OpenCV library
#include <opencv2/objdetect.hpp>
#include <atomic>

/*****        LOCAL INCLUDE         *****/
#include "config.hpp"
#include "utils/Utils.hpp"
#include "utils/Window.hpp"
#include "webserver/Webserver.hpp"

// Worker
#include "pipeline/Pipeline.hpp"
#include "pipeline/workers/WindowWorker.hpp"
#include "pipeline/CameraProvider.hpp"
#include "pipeline/TestProvider.hpp"
#include "pipeline/workers/FilterColorizeWorker.hpp"
#include "pipeline/workers/FilterTemporalWorker.hpp"
#include "pipeline/workers/FilterLineWorker.hpp"
#include "pipeline/workers/FilterInterpolatorWorker.hpp"
#include "pipeline/workers/FilterDiscreticiser.hpp"
#include "pipeline/workers/FilterScaleWorker.hpp"
#include "pipeline/workers/FilterResourcePlacementWorker.hpp"
#include "pipeline/workers/FilterAssetOverlayWorker.hpp"
#include "pipeline/workers/FilterTranslatorWorker.hpp"
#include "pipeline/workers/FilterDifferenceWorker.hpp"
#include "pipeline/workers/FilterStripeWorker.hpp"
#include "pipeline/workers/FilterVisualCutWorker.hpp"
#include "pipeline/game/GameLogicWorker.hpp"
#include "pipeline/game/GameDrawWorker.hpp"
#include "pipeline/workers/FilterPerlinWorker.hpp"

/*****           MISC               *****/
#include <iostream>
#include <chrono>
#include <regex>

cv::Point2i translation_vec = {-3, -2};
cv::Size scalar_kernel = {(int)(STREAM_WIDTH), (int)(0.993 * STREAM_HEIGHT)};
volatile bool stay_in_calib = true;

/**
 * @fn int main()
 * @brief Responsible for the Window, Pipeline and communication between Camera and Output Image.
 * @param none
 * @returns Error Code
 */

__ARTE_INIT_ int main(int argc, char **argv)
{
    std::srand((unsigned)std::time(nullptr));
    std::cout << "Starting DepthCamera, running in " << std::this_thread::get_id() << std::endl;

    Provider *camera_provider = new CameraProvider{"Camera_Provider"};

    //! Temp
    Filter::DiscreticiserWorker discreticiser_worker{"Filter_Discreticiser_Worker"};

    if(argc > 1)
    {
        cv::Mat mat = res_load_depth_mat(argv[1]);
        if(!mat.empty())
        {
            camera_provider->stop();
            delete camera_provider;
            const short m_disc_start = 1150;
            const short m_disc_end = 1350;
            const short m_lin_steps = DISCRETE_STEPS;
            mat.convertTo(mat, CV_16U, -13.33333333, 1350);

            camera_provider = new Test_Provider{"Test_Provider", mat};
        }
    }

    Window window{"DUNE", MONITOR_BEAMER_FIX};
#if WEB_UI
#else
    Window twindow{"DUNE Touch", MONITOR_TOUCHDISPLAY_FIX};
#endif
    // Window
    WindowWorker window_worker{"Window_Worker", &window};
    Filter::TemporalWorker temporal_worker{"Filter_Temporal_Worker"};
    Filter::ColorizeWorker colorize_worker{"Filter_Colorize_Worker", Filter::ColorizeWorker::COLORIZE_TYPE_e::DEFAULT};
    Filter::ColorizeWorker diff_colorize_worker{"Filter_Colorize_Diff_Worker", Filter::ColorizeWorker::COLORIZE_TYPE_e::DIFFERENCE};
    Filter::ColorizeWorker perlin_colorize_worker{"Filter_Colorize_Perlin_Worker", Filter::ColorizeWorker::COLORIZE_TYPE_e::PERLIN};
    Filter::LineWorker line_worker{"Filter_Line_Worker"};
    Filter::InterpolatorWorker interpolator_worker{"Filter_Interpolator_Worker"};
    //> temp > Filter::DiscreticiserWorker discreticiser_worker{"Filter_Discreticiser_Worker"};
    discreticiser_worker.bind_to_window(window);
    Filter::ScaleWorker scale_worker{"Filter_Scale_Worker"};
    Filter::ResourcePlacementWorker resource_placement{"Resource_Placement_Worker", BC_Trees, tree_asmak_WIDTH, tree_asmak_HEIGHT, tree_asmak_DATA};
    Filter::AssetOverlayWorker asset_overlay{"Asset_Overlay_Filter", BC_Trees};
    Filter::DifferenceWorker difference_worker{"Filter_Difference_Worker"};
    Filter::StripeWorker stripe_worker{"Filter_Stripe_Worker"};
    TranslatorWorker translator_worker{"Translator_Worker"};
    Filter::VisualCutWorker visual_cut_worker{"Visual_Cut_Worker"};
    Simulation::GameLogicWorker game_logic_worker{"Game_Logic_Worker"};
    Simulation::GameDrawWorker game_draw_worker{"Game_Draw_Worker", &game_logic_worker};

    Filter::PerlinWorker perlin_worker{"Perlin_Worker", false};
    Filter::DifferenceWorker perlin_difference_worker{"Filter_Perlin_Difference_Worker", true};

    Pipeline pipeline_minecraft{camera_provider};
    Pipeline pipeline_smooth{camera_provider};
    Pipeline pipeline_difference{camera_provider};
    Pipeline pipeline_stripe{camera_provider};
    Pipeline pipeline_perlin{camera_provider};

    pipeline_smooth.push_worker(&discreticiser_worker);
    pipeline_smooth.push_worker(&scale_worker);
    pipeline_smooth.push_worker(&translator_worker);
    pipeline_smooth.push_worker(&visual_cut_worker);
    pipeline_smooth.push_worker(&temporal_worker);
    pipeline_smooth.push_worker(&game_logic_worker);
    pipeline_smooth.push_worker(&colorize_worker);
    pipeline_smooth.push_worker(&interpolator_worker);
    pipeline_smooth.push_worker(&game_draw_worker);
    pipeline_smooth.push_worker(&window_worker);


    pipeline_minecraft.push_worker(&discreticiser_worker);
    pipeline_minecraft.push_worker(&scale_worker);
    pipeline_minecraft.push_worker(&translator_worker);
    pipeline_minecraft.push_worker(&visual_cut_worker);
    pipeline_minecraft.push_worker(&temporal_worker);
    pipeline_minecraft.push_worker(&asset_overlay);
    pipeline_minecraft.push_worker(&line_worker);
    pipeline_minecraft.push_worker(&colorize_worker);
    pipeline_minecraft.push_worker(&resource_placement);
    pipeline_minecraft.push_worker(&window_worker);

    pipeline_difference.push_worker(&discreticiser_worker);
    pipeline_difference.push_worker(&scale_worker);
    pipeline_difference.push_worker(&translator_worker);
    pipeline_difference.push_worker(&visual_cut_worker);
    pipeline_difference.push_worker(&temporal_worker);
    pipeline_difference.push_worker(&difference_worker);
    pipeline_difference.push_worker(&diff_colorize_worker);
    pipeline_difference.push_worker(&interpolator_worker);
    pipeline_difference.push_worker(&window_worker);

    pipeline_stripe.push_worker(&discreticiser_worker);
    pipeline_stripe.push_worker(&scale_worker);
    pipeline_stripe.push_worker(&translator_worker);
    pipeline_stripe.push_worker(&visual_cut_worker);
    pipeline_stripe.push_worker(&temporal_worker);
    pipeline_stripe.push_worker(&stripe_worker);
    pipeline_stripe.push_worker(&colorize_worker);
    pipeline_stripe.push_worker(&interpolator_worker);
    pipeline_stripe.push_worker(&window_worker);

    pipeline_perlin.push_worker(&perlin_worker);
    pipeline_perlin.push_worker(&discreticiser_worker);
    pipeline_perlin.push_worker(&scale_worker);
    pipeline_perlin.push_worker(&translator_worker);
    pipeline_perlin.push_worker(&visual_cut_worker);
    pipeline_perlin.push_worker(&temporal_worker);
    pipeline_perlin.push_worker(&perlin_difference_worker);
    pipeline_perlin.push_worker(&perlin_colorize_worker);
    pipeline_perlin.push_worker(&interpolator_worker);
    pipeline_perlin.push_worker(&window_worker);

    pipeline_smooth.start();

#if WEB_UI

    Webserver webserver{};
    webserver.create();
    webserver.listen(8080);

    auto filter = webserver.get_data().filter;
    while (window)
    {
        auto new_filter = webserver.get_data().filter;
        if (new_filter == filter)
            continue;
        filter = new_filter;

        clog(info) << "Received new HTTP Data - Initiating Pipeline Switch!" << std::endl;
        // if(new_filter.is_filter == true) { //< Hier bitte ein zweites flag erstellen dass hier ein conditional zwischen "neuer filter" und "perlin settings" unterscheiden kann
        pipeline_minecraft.stop();
        pipeline_smooth.stop();
        pipeline_difference.stop();
        pipeline_stripe.stop();
        pipeline_perlin.stop();
        clog(info) << "Switching to -> ";
        switch (filter)
        {
        case Data::Filter::NORMAL:
            clog(info) << "SMOOTH FILTER PIPELINE" << std::endl;
            pipeline_smooth.start();
            break;
        case Data::Filter::BLOCKCRAFT:
            clog(info) << "BLOCKCRAFT FILTER PIPELINE" << std::endl;
            pipeline_minecraft.start();
            break;
        case Data::Filter::DIFFERENCE:
            clog(info) << "DIFFERENCE FILTER PIPELINE" << std::endl;
            pipeline_difference.start();
            break;
        case Data::Filter::STRIPE:
            clog(info) << "STRIPE FILTER PIPELINE" << std::endl;
            pipeline_stripe.start();
            break;
        case Data::Filter::PERLIN:
            clog(info) << "PERLIN FILTER PIPELINE" << std::endl;
            pipeline_perlin.start();
            break;
        }
    }

    pipeline_minecraft.stop();
    pipeline_smooth.stop();
    pipeline_difference.stop();
    pipeline_stripe.stop();
#else
    int state = twindow.get_dat();
    twindow.render_matrix(getCaptureIndex(state));

    while (window && twindow)
    {
        if (state != twindow.get_dat())
        {
            twindow.cooldown = true;
            state = twindow.get_dat();
            twindow.render_matrix(getCaptureIndex(state));
            twindow.operator bool();

            pipeline_minecraft.stop();
            pipeline_smooth.stop();
            pipeline_difference.stop();
            pipeline_stripe.stop();

            if (state == 0)
            {
                pipeline_minecraft.start();
            }
            else if (state == 1)
            {
                pipeline_smooth.start();
            }
            else if (state == 2)
            {
                pipeline_difference.start();
            }
            else if (state == 3)
            {
                pipeline_stripe.start();
            }

            difference_worker.reset_save();
            twindow.cooldown = false;
        }
        if (twindow.docapture)
        {
            twindow.docapture = false;
            cv::Mat mat = window.capture();
            saveMatAsFile(mat, "CAPTURE", "./capture.dres");
        }
    }

    window.close();
    twindow.close();
    while (window || twindow)
        ;
#endif
    camera_provider->stop();
    delete camera_provider;
}
