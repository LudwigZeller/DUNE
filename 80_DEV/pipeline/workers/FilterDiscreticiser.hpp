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
    const short m_disc_start = DISCRETE_START;
    const short m_disc_end = DISCRETE_END;
    const short m_lin_steps = DISCRETE_STEPS;
    cv::Mat m_tmp_matrix;
    bool m_do_capture = false;

public:
    explicit DiscreticiserWorker(std::string id): Worker(std::move(id), MatIOType::SHORT_16, MatIOType::CHAR_8)
    { /* No extra construction required */ }

protected:

    void start_up() override
    {}

    void work() override
    {
        if(this->m_do_capture) [[unlikely]]
        {
            this->m_do_capture = false;
            dres_save_depth_mat("output", this->m_work_matrix);
        }

        const static double _scalc1 = this->m_disc_end - m_disc_start;
        const static double _scalc2 = 1.0 / (_scalc1 / ((double)this->m_lin_steps - 2.0));

        this->m_tmp_matrix = std::move(this->m_work_matrix);
        this->m_work_matrix = cv::Mat(this->m_tmp_matrix.size(), CV_8U);

        this->m_work_matrix.forEach<uchar>([&](uchar &c, const int *pos){
            double t = _scalc2 * (this->m_tmp_matrix.at<short>(pos)- this->m_disc_start);
            c = (DISCRETE_STEPS - 1) - (uchar) ___max_(___min_(t + 1, this->m_lin_steps - 1), 0);
        });
    }

    void capture()
    {
        this->m_do_capture = true;
    }
};

};
