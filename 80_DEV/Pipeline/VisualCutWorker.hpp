#pragma once

#include "Worker.hpp"
#include "CalibRTE.hpp"

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
#define BORDER_CUTOFF_TL_Y              (9)
#define BORDER_CUTOFF_TR_Y              (0)
#define BORDER_CUTOFF_B_Y               (700)
#define BORDER_CUTOFF_BL_X              (202)
#define BORDER_CUTOFF_BR_X              (1152)
#define BORDER_CUTOFF_TL_X              (206)
#define BORDER_CUTOFF_TR_X              (1130)

private:
    cv::Mat m_cutoff_template = cv::Mat::zeros(cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_8U);
    cv::Mat m_work_storage;

    const cv::Rect m_cutoff_rect{
        CUTOFF_LEFT, CUTOFF_TOP,
        CUTOFF_RIGHT - CUTOFF_LEFT, CUTOFF_BOT - CUTOFF_TOP
    };

public:
    explicit VisualCutWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        cv::line(m_cutoff_template, cv::Point2i{BORDER_CUTOFF_TL_X, BORDER_CUTOFF_TL_Y}, cv::Point2i{BORDER_CUTOFF_TR_X, BORDER_CUTOFF_TR_Y}, 13, 4);
        cv::line(m_cutoff_template, cv::Point2i{BORDER_CUTOFF_TR_X, BORDER_CUTOFF_TR_Y}, cv::Point2i{BORDER_CUTOFF_BR_X, BORDER_CUTOFF_B_Y }, 13, 4);
        cv::line(m_cutoff_template, cv::Point2i{BORDER_CUTOFF_BR_X, BORDER_CUTOFF_B_Y }, cv::Point2i{BORDER_CUTOFF_BL_X, BORDER_CUTOFF_B_Y }, 13, 4);
        cv::line(m_cutoff_template, cv::Point2i{BORDER_CUTOFF_BL_X, BORDER_CUTOFF_B_Y }, cv::Point2i{BORDER_CUTOFF_TL_X, BORDER_CUTOFF_TL_Y}, 13, 4);
    }

protected:
    void start_up() override
    { /* No required */ }

    void work() override
    {
        this->m_work_storage = std::move(this->m_work_matrix);
        this->m_work_matrix = this->m_cutoff_template.clone();
        cv::copyTo(this->m_work_storage(m_cutoff_rect),
            this->m_work_matrix(m_cutoff_rect), cv::Mat{});
    }
};

};
