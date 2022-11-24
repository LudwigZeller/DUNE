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
protected:
    cv::Mat m_temp_stor;
public:
    explicit LineWorker(std::string id): Worker{std::move(id)}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

    void start_up() override
    { /* No start up required for now */ }

    #define __lw_min(a,b) ((a) < (b) ? (a) : (b))
    #define __lw_max(a,b) ((a) > (b) ? (a) : (b))

    #define __ldetail (2)

    void work() override
    {
        m_temp_stor = m_work_matrix.clone();
        m_work_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos){
            int ym = __lw_min(pos[0] + __ldetail, m_temp_stor.size().height - 1);
            int xm = __lw_min(pos[1] + __ldetail, m_temp_stor.size().width - 1);
            cv::Vec3b &cmp = m_temp_stor.at<cv::Vec3b>(pos);

            for(int y = __lw_max(pos[0] - __ldetail, 0); y <= ym; y += __ldetail)
            {
                for(int x = __lw_max(pos[1] - __ldetail, 0); x <= xm; x += __ldetail)
                {
                    if(cmp != m_temp_stor.at<cv::Vec3b>(y, x))
                    {
                        pixel = cv::Vec3b{255,255,255};
                        return;
                    }
                }
            }
        });
        
    }
};

};
