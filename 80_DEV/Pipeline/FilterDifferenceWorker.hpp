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
        //! Statement irrelevant in normal mode
        if(linger_frame) val_linger_frame = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
    }

    void work() override
    {
        //!! Statement to save the first available Frame, has secondary delay statement
        //!! when in "Perlin mode"
        if(save_first && !(linger_frame && val_linger_frame >= PERLIN_LINGER_LENGTH))
        {
            this->save_first = false;
            this->m_reference = this->m_work_matrix.clone();
        }

        //!! Statement irrelevant in normal mode
        if(val_linger_frame) val_linger_frame--;
        if(!val_linger_frame)
        //^^

            for(int y = 0; y < STREAM_HEIGHT; y++)
                for(int x = 0; x < STREAM_WIDTH; x++)
                {
                    signed char &value_is = this->m_work_matrix.at<signed char>(y,x);
                    const signed char &value_should = this->m_reference.at<signed char>(y,x);

                    value_is = (value_should > 0 && value_should < DISCRETE_STEPS - 1) ?
                               (___min_(14, ___max_(1, 7 - value_is + value_should))) :
                               value_should;
                }
    }


};

};
