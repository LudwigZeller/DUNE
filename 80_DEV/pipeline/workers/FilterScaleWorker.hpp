#pragma once

#include "../Worker.hpp"
#include "../../calibRTE.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ScaleWorker : public Worker
 * @brief Responsible for de-3d-izing
 */
class ScaleWorker : public Worker
{
public:
    ScaleWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* No start up required for now */ }

    void work() override
    {
        const static cv::Rect resize_kernel{0, 0, scalar_kernel.width, scalar_kernel.height};
        cv::resize(this->m_work_matrix, this->m_work_matrix(resize_kernel), scalar_kernel);
    }
};

};
