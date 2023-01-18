#pragma once

#include "Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class DiscreticiserWorker : public Worker
 * @brief Discretization of meter data. 
*/
class DifferenceWorker : public Worker
{
protected:
    cv::Mat m_reference;
    bool save_first = true;

public:
    explicit DifferenceWorker(std::string id): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8)
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

    void reset_save()
    {
        this->save_first = true;
    }

protected:
    void start_up() override
    {
        /* No startup required */
    }

    void work() override
    {
        if(save_first)
        {
            this->save_first = false;
            this->m_reference = this->m_work_matrix.clone();
        }

        this->m_work_matrix.forEach<uchar>([&](uchar &pixel, const int *pos)
        {
        #define __abs(a) (((a) > 0) ? (a) : -(a))
            pixel = __abs((signed char) pixel - (signed char) this->m_reference.at<uchar>(pos));
            pixel = pixel > 1 ? pixel - 1 : 0;
        });
    }


};

};
