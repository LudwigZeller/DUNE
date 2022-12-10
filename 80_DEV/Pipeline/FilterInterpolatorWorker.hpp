#pragma once

#include "Worker.hpp"

const static cv::Size ksize{9,9};

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class InterpolatorWorker : public Worker
{
public:
    InterpolatorWorker(std::string id): Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

protected:
    void start_up() override
    { /* Start up not required for now */ }

    void work() override
    {
        cv::Mat tmp = this->m_work_matrix.clone();
        cv::blur(tmp, this->m_work_matrix, ksize);
    }
};

};
