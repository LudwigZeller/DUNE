#pragma once

#include "../Worker.hpp"
#include "../../calibRTE.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class VisualCutWorker : public Worker
{
#define BORDER_CUTOFF_TOP 23
#define BORDER_CUTOFF_BOT 681
#define BORDER_CUTOFF_LEFT 230
#define BORDER_CUTOFF_RIGHT 1125

#define CUTOFF_TOP 53
#define CUTOFF_BOT 681
#define CUTOFF_LEFT 250
#define CUTOFF_RIGHT 1100
#define CUTOFF_FRAME_STRENGTH 5
private:
    cv::Mat m_cutoff_template = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_8U);
    cv::Mat m_work_storage;

    const cv::Rect m_cutoff_frame_rect{
        BORDER_CUTOFF_LEFT - CUTOFF_FRAME_STRENGTH, BORDER_CUTOFF_TOP - CUTOFF_FRAME_STRENGTH,
        BORDER_CUTOFF_RIGHT - BORDER_CUTOFF_LEFT + 2 * CUTOFF_FRAME_STRENGTH, BORDER_CUTOFF_BOT - BORDER_CUTOFF_TOP + 2 * CUTOFF_FRAME_STRENGTH
    };
    const cv::Rect m_cutoff_inner_rect{
        BORDER_CUTOFF_LEFT, BORDER_CUTOFF_TOP,
        BORDER_CUTOFF_RIGHT - BORDER_CUTOFF_LEFT, BORDER_CUTOFF_BOT - BORDER_CUTOFF_TOP
    };
    const cv::Rect m_cutoff_rect{
        CUTOFF_LEFT, CUTOFF_TOP,
        CUTOFF_RIGHT - CUTOFF_LEFT, CUTOFF_BOT - CUTOFF_TOP
    };

public:
    explicit VisualCutWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        this->m_cutoff_template(m_cutoff_frame_rect).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 14;
        });
        this->m_cutoff_template(m_cutoff_inner_rect).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 0;
        });
    }

protected:
    void start_up() override
    { /* No required */ }

    void work() override
    {
        this->m_work_storage = std::move(this->m_work_matrix);
        this->m_work_matrix = this->m_cutoff_template.clone();
        cv::copyTo(this->m_work_storage(m_cutoff_rect), this->m_work_matrix(m_cutoff_rect), cv::Mat{});
    }
};

};
