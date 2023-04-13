#pragma once

#include "../Worker.hpp"
#include "../../calibRTE.hpp"

class TranslatorWorker : public Worker
{
public:
    explicit TranslatorWorker(std::string id): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8)
    { /* No further construction required */ }

protected:
    inline friend cv::Point2i operator-(const cv::Point2i &c) {
        return {-c.x, -c.y};
    }

    inline void start_up() override
    {
        
    }

    inline void work() override
    {
        mat_meta_translate(this->m_work_matrix, translation_vec);
    }
};
