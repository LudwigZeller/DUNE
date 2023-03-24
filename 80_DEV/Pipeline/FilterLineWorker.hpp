#pragma once

#include "Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class LineWorker : public Worker
 * @brief Temporary line colorization worker
*/
class LineWorker : public Worker
{
public:
    explicit LineWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* No start up required for now */ }

    #define __ldetail (1)
    void work() override
    {
        const static cv::Rect line_proc_rect{CUTOFF_LEFT, CUTOFF_TOP, CUTOFF_RIGHT - CUTOFF_LEFT + 1, CUTOFF_BOT - CUTOFF_TOP + 1};

        this->m_work_matrix(line_proc_rect).forEach<uchar>([&](uchar &pixel, const int *pos)
        {
            int rt = pixel < (this->m_work_matrix.at<uchar>(pos[0] - __ldetail, pos[1]) % (DISCRETE_STEPS)) ||
                     pixel < (this->m_work_matrix.at<uchar>(pos[0] + __ldetail, pos[1]) % (DISCRETE_STEPS)) ||
                     pixel < (this->m_work_matrix.at<uchar>(pos[0], pos[1] - __ldetail) % (DISCRETE_STEPS)) ||
                     pixel < (this->m_work_matrix.at<uchar>(pos[0], pos[1] + __ldetail) % (DISCRETE_STEPS));
            
            //! LINE_MASK = 0x40
            pixel |= LINE_MASK * rt;
        });
    }
};

};
