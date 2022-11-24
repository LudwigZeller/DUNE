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
    cv::Mat m_temporal1,
            m_temporal2,
            m_temporal3;

public:
    TemporalWorker(std::string id): Worker{std::move(id)}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

protected:
    void start_up() override
    {
        this->m_temporal1 = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_64F);
        this->m_temporal2 = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_64F);
        this->m_temporal3 = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_64F);
    }

    void work() override
    {

        this->m_work_matrix += this->m_temporal1 + this->m_temporal2 + this->m_temporal3;
        this->m_work_matrix /= 4.0;

        this->m_temporal3.release();
        this->m_temporal3 = std::move(this->m_temporal2);
        this->m_temporal2 = std::move(this->m_temporal1);
        this->m_temporal1 = this->m_work_matrix.clone();
    }
};

};