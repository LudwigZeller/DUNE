#pragma once

#include "../Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class DiscreticiserWorker : public Worker
 * @brief Discretization of meter data. 
*/
class StripeWorker : public Worker
{
public:
    explicit StripeWorker(std::string id): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8)
    { /* No extra construction required */ }

protected:
    void start_up() override
    {
        /* No startup required */
    }

    void work() override
    {
        this->m_work_matrix.forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = (pixel > 0 && pixel < (DISCRETE_STEPS - 1)) ? (
                (pixel % 2) ? (
                    ((___abs_(pos[0] - pos[1]) / STRIPE_WIDTH) % 2) ? pixel : (pixel - 1)
                ) : (
                    (((pos[0] + pos[1]) / STRIPE_WIDTH) % 2) ? pixel : (pixel - 1)
                )
            ) : pixel;
        });
    }
};

};
