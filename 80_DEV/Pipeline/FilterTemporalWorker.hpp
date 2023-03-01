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
    #define num_buffer 12
    std::vector<cv::Mat> buffer{num_buffer};
    const double i_num_buffer = 1.0 / num_buffer;

public:
    TemporalWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        for(auto i = buffer.begin(); i != buffer.end(); i++)
        {
            *i = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_8U);
        }
        
    }

protected:
    void start_up() override
    {
    }

    void work() override
    {
        std::copy(buffer.begin() + 1, buffer.end(), buffer.begin());
        buffer.back() = this->m_work_matrix.clone();

        for(auto i = buffer.begin(); i + 1 != buffer.end(); i++)
            this->m_work_matrix += *i;
        this->m_work_matrix *= i_num_buffer;
        //cv::blur(this->m_work_matrix, buffer.back(), cv::Size{3,3});
    }
};

};
