#include "DManager.hpp"
#include "../config.hpp"
#include "../Utilities/utils.hpp"
#include "../RTE.hpp"

using namespace std::chrono;

namespace DManager
{

class DPipelineManager : public DManager
{
public:
    DPipelineManager() : DManager(DManagerFlags{
        .can_skip = false,
        .log_lag = DEBUG_ON_SCREEN,
        .target_period = milliseconds(int(1000.0f / FPS)),
        .delay_target_period = milliseconds(0),
        .log_ela = false,
        .name = "Pipeline Manager"
    })
    {
    }

    cv::Mat logo;
    #if DEBUG_ON_SCREEN
    std::string debug_str;
    #endif

    virtual void init_user()
    {
        std::cout << "Starting Pipeline manager" << std::endl;
        logo = cv::imread("DuneLogo.png");
        debug_str = "";
        while(!RTE::window.initialized());

        while(!RTE::window);
        RTE::window_open = true;
    }

    virtual void run_user()
    {
        if(!RTE::window)
        {
            RTE::window_open = false;
            RTE::exit_code_pipeline = 0;
            RTE::terminate_all = true;
            return;
        }

        if(RTE::camera_connected)
        {
            RTE::draw = 1;
            //draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), RTE::window_out_matrix);
        }
        else
        {
            RTE::draw = 2;
            //draw_frame(cv::Size((int) RTE::window.width(), (int) RTE::window.height()), logo);
            //TODO: Draw Dune logo Ludwig
        }

        //!! Compiler-level configurable on-screen debug information
        #if DEBUG_ON_SCREEN
        debug_str = "Last ela: " + this->get_millis();
        draw_text_debug(10, 10, debug_str.c_str());
        #endif
    }
};

};
