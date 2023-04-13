#pragma once

#include "../Worker.hpp"
#include "../../utils/Window.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class DiscreticiserWorker : public Worker
 * @brief Discretization of meter data. 
*/
class DiscreticiserWorker : public Worker
{
protected:
    // Todo: These parameters 
    const short m_disc_start = 1150;
    const short m_disc_end = 1350;
    const short m_lin_steps = DISCRETE_STEPS;
    cv::Mat tmp;
    bool do_capture = false;

public:
    explicit DiscreticiserWorker(std::string id): Worker(std::move(id), MatIOType::SHORT_16, MatIOType::CHAR_8)
    { /* No extra construction required */ }

    void bind_to_window(Window &w)
    {
        w.bind_capture_adress([this](){this->capture();});
    }

protected:

    void start_up() override
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    void work() override
    {
        const static double _scalc1 = m_disc_end - m_disc_start;
        const static double _scalc2 = 1.0 / (_scalc1 / (m_lin_steps - 2));

        tmp = std::move(this->m_work_matrix);
        this->m_work_matrix = cv::Mat(tmp.size(), CV_8U);

        this->m_work_matrix.forEach<uchar>([&](uchar &c, const int *pos){
            double t = _scalc2 * (this->tmp.at<short>(pos)- m_disc_start);
            c = (DISCRETE_STEPS - 1) - (uchar) ___max_(___min_(t + 1, this->m_lin_steps - 1), 0);
            //c = ~(~(1 << c) + 1);
        });

        if(do_capture)
        {
            do_capture = false;
            dres_save_depth_mat("output", this->m_work_matrix);
        }
    }

    void capture()
    {
        do_capture = true;
    }
};

};
