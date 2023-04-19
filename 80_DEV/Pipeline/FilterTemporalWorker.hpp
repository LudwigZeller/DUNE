#pragma once

#include "Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class TemporalWorker : public Worker
{
protected:
    std::vector<cv::Mat> buffer{TEMPORAL_BUFFER_LENGTH};
    const double i_num_buffer = 1.0 / TEMPORAL_BUFFER_LENGTH;

public:
    TemporalWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        for(auto i = buffer.begin(); i != buffer.end(); i++)
        {
            *i = cv::Mat(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_8U, 15);
        }
        
    }

protected:
    void start_up() override
    {
    }

    void work() override
    {
        //!! Shifting statement
        std::copy(buffer.begin() + 1, buffer.end(), buffer.begin());
        buffer.back() = this->m_work_matrix.clone();

        //!! Summing
        for(cv::Mat &m : this->buffer)
            this->m_work_matrix += m;

        //!! Division and rounding
        this->m_work_matrix.convertTo(this->m_work_matrix, CV_16F, i_num_buffer, 0.49);
        this->m_work_matrix.convertTo(this->m_work_matrix, CV_8U);
    }
};

};
