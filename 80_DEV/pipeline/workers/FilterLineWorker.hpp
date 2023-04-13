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
public:
    explicit LineWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* No start up required for now */ }

    #define __ldetail (1)

    void work() override
    {
        this->m_temp_stor = this->m_work_matrix.clone();
        this->m_work_matrix.forEach<uchar>([&](uchar &pixel, const int *pos){
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

        //!! This filter leads to blocky and thick lines, which create a pixely image!
        /*m_temp_stor = m_work_matrix.clone();
        m_work_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos){
            int ym = __lw_min(pos[0] + __ldetail, m_temp_stor.rows - 1);
            int xm = __lw_min(pos[1] + __ldetail, m_temp_stor.cols - 1);
            int yn = __lw_max(pos[0] - __ldetail, 0);
            int xn =  __lw_max(pos[1] - __ldetail, 0);

            cv::Vec3b &cmp = this->m_temp_stor.at<cv::Vec3b>(pos);
            bool rt = 
                   cmp != this->m_temp_stor.at<cv::Vec3b>(yn,pos[1])
                || cmp != this->m_temp_stor.at<cv::Vec3b>(ym,pos[1])
                || cmp != this->m_temp_stor.at<cv::Vec3b>(pos[0],xn)
                || cmp != this->m_temp_stor.at<cv::Vec3b>(pos[0],xm);
            
            if(rt) pixel *= 0.5;

            for(int y = __lw_max(pos[0] - __ldetail, 0); y <= ym; y += __ldetail)
            {
                for(int x = __lw_max(pos[1] - __ldetail, 0); x <= xm; x += __ldetail)
                {
                    if(cmp != m_temp_stor.at<cv::Vec3b>(y, x))
                    {
                        pixel *= 0.5;
                        //pixel = cv::Vec3b{255,255,255};
                        return;
                    }
                }
            }
        });*/
        
    }
};

};
