#pragma once

#include "Worker.hpp"
#include "CalibRTE.hpp"

double __sw_cord_factor = 1.0 / std::sqrt((double) (STREAM_WIDTH * STREAM_WIDTH) * 0.25 + (double) (STREAM_HEIGHT * STREAM_HEIGHT) * 0.25);
double __sw_x_offset = -STREAM_WIDTH * 0.5;
double __sw_y_offset = -STREAM_HEIGHT * 0.5;
double __sw_disc_factor = 0.5 / (double) DISCRETE_STEPS;

inline double get_alt_at(const int *rpos, uchar &val)
{
    double rposx = __sw_cord_factor * ((double)rpos[1] + __sw_x_offset);
    double rposy = __sw_cord_factor * ((double)rpos[0] + __sw_y_offset);
    double raw_val = rposx * rposx + rposy * rposy;
    return raw_val * __sw_disc_factor * (double) val;
}

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
