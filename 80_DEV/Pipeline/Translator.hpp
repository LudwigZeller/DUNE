#pragma once

#include "Worker.hpp"
#include "CalibRTE.hpp"

namespace Filter
{
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
    { /* none required */ 
        
    }

    void work() override
    {
        bool x_smaller_0 = 0 > translation_vec.x;
        bool y_smaller_0 = 0 > translation_vec.y;
        int x_area_cp = STREAM_WIDTH + (x_smaller_0 ? translation_vec.x : (-translation_vec.x));
        int y_area_cp = STREAM_HEIGHT + (y_smaller_0 ? translation_vec.y : (-translation_vec.y));

        cv::Rect src_rect
        {
            !x_smaller_0 ? 0 : -translation_vec.x,
            !y_smaller_0 ? 0 : -translation_vec.y,
            x_area_cp, y_area_cp
        };

        cv::Rect dst_rect
        {
            !x_smaller_0 ? translation_vec.x : 0,
            !y_smaller_0 ? translation_vec.y : 0,
            x_area_cp, y_area_cp
        };

        cv::copyTo(this->m_work_matrix(src_rect), this->m_work_matrix(dst_rect), cv::Mat());
    }
};
};
