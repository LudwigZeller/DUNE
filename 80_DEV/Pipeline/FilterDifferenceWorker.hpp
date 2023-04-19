#pragma once

#include "Worker.hpp"
#include "AssetRTE.hpp"

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
            for(int y = CUTOFF_TOP; y <= CUTOFF_BOT; y++)
                for(int x = CUTOFF_LEFT; x <= CUTOFF_RIGHT; x++)
                {
                    uchar &is = this->m_work_matrix.at<uchar>(y,x);
                    const uchar &be = this->m_reference.at<uchar>(y,x);

                    is = (be > 0 && be < DISCRETE_STEPS - 1) ? (___min_(14, ___max_(1, 7 - (signed char) is + (signed char) be))) : be;
                }
    }


};

};
