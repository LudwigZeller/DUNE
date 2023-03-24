#pragma once

#include "Worker.hpp"

const static cv::Size ksize{9,9};

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class InterpolatorWorker : public Worker
{
private:
    cv::Size m_scaleup{2,2};

public:
    InterpolatorWorker(std::string id): Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* Start up not required for now */ }

    void work() override
    {
        //cv::upsa
        //cv::resize(this->m_work_matrix, this->m_work_matrix, cv::Size{}, 2.0, 2.0, cv::InterpolationFlags::INTER_LINEAR);
        //cv::GaussianBlur(this->m_work_matrix, this->m_work_matrix, ksize, 0, 0, cv::BorderTypes::BORDER_REPLICATE);
        //cv::blur(this->m_work_matrix, this->m_work_matrix, ksize);
        cv::Mat m1 = this->m_work_matrix(cv::Rect{
            CUTOFF_LEFT,CUTOFF_TOP,(CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2 + 1,CUTOFF_BOT - CUTOFF_TOP + 1
        });
        cv::Mat m2 = this->m_work_matrix(cv::Rect{
            (CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2 + 1 + CUTOFF_LEFT, CUTOFF_TOP, (CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2 + 1, CUTOFF_BOT - CUTOFF_TOP + 1
        });
        std::thread t1{[&]{
            cv::medianBlur(m1,m1,5);
        }};
        std::thread t2{[&]{
            cv::medianBlur(m2,m2,5);
        }};

        t1.join(); t2.join();

        //cv::medianBlur(this->m_work_matrix,this->m_work_matrix,3);
    }
};

};
