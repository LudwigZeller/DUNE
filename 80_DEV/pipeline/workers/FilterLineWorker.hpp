#pragma once

#include "../Worker.hpp"

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
protected:
    cv::Mat m_temp_stor;
    cv::Rect m_mat_frame{CUTOFF_LEFT, CUTOFF_TOP, CUTOFF_RIGHT - CUTOFF_LEFT + 1, CUTOFF_BOT - CUTOFF_TOP + 1};

public:
    explicit LineWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* No start up required for now */ }

    #define __ldetail (1)

    void work() override
    {
        this->m_temp_stor = this->m_work_matrix(m_mat_frame).clone();
        this->m_work_matrix(m_mat_frame).forEach<uchar>([&](uchar &pixel, const int *pos){
            int ym = ___min_(pos[0] + __ldetail, m_temp_stor.rows - 1);
            int xm = ___min_(pos[1] + __ldetail, m_temp_stor.cols - 1);
            int yn = ___max_(pos[0] - __ldetail, 0);
            int xn = ___max_(pos[1] - __ldetail, 0);

            uchar &cmp = this->m_temp_stor.at<uchar>(pos);
            int rt = 
                   cmp < this->m_temp_stor.at<uchar>(yn,pos[1])
                || cmp < this->m_temp_stor.at<uchar>(ym,pos[1])
                || cmp < this->m_temp_stor.at<uchar>(pos[0],xn)
                || cmp < this->m_temp_stor.at<uchar>(pos[0],xm);
            
            pixel |= LINE_MASK * rt;
        });
    }
};

};
