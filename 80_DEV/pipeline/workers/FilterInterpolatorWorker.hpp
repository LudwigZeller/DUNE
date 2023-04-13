#pragma once

#include "../Worker.hpp"

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
private:
    cv::Size m_scaleup{2,2};

public:
    InterpolatorWorker(std::string id): Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* Start up not required for now */ }

    void work() override
    {
        // TODO
    }
};

};
