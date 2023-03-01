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
    bool linger_frame;
    int val_linger_frame = 0;

public:
    explicit DifferenceWorker(std::string id, bool linger_frame = false): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8), linger_frame(linger_frame)
    { /* No extra construction required */ }

    void reset_save()
    {
        this->save_first = true;
        if(linger_frame) val_linger_frame = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
    }

protected:
    void start_up() override
    {
        this->save_first = true;
        if(linger_frame) val_linger_frame = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
    }

    void work() override
    {
        if(save_first && !(linger_frame && val_linger_frame >= PERLIN_LINGER_LENGTH))
        {
            this->save_first = false;
            this->m_reference = this->m_work_matrix.clone();
        }

        if(val_linger_frame) val_linger_frame--;
        if(!val_linger_frame)
            this->m_work_matrix.forEach<uchar>([&](uchar &pixel, const int *pos)
            {
                pixel = 7 + (signed char) this->m_reference.at<uchar>(pos) - (signed char) pixel;
            });
    }


};

};
