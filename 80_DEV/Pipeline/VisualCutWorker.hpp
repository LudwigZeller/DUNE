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
public:
    explicit VisualCutWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

protected:
    void start_up() override
    { /* No required */ }

    void work() override
    {
        this->m_work_matrix(
            cv::Rect{
                0,0,STREAM_WIDTH,STREAM_HEIGHT/2-300
            }
        ).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 0;
        });

        this->m_work_matrix(
            cv::Rect{
                0,STREAM_HEIGHT/2+349,STREAM_WIDTH,STREAM_HEIGHT/2-350
            }
        ).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 0;
        });

        this->m_work_matrix(
            cv::Rect{
                0,STREAM_HEIGHT/2-300,STREAM_WIDTH/2 - 400,650
            }
        ).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 0;
        });

        this->m_work_matrix(
            cv::Rect{
                STREAM_WIDTH/2 +449,STREAM_HEIGHT/2-300,STREAM_WIDTH/2 - 450,650
            }
        ).forEach<uchar>([](uchar &pixel, const int *pos)
        {
            pixel = 0;
        });
    }
};

};
