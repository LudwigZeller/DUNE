#pragma once

#include "Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class ColorizeWorker : public Worker
{
public:
    const cv::Vec3b black{0,0,0};
    const cv::Vec3b blue{255,0,0};
    const cv::Vec3b dark_blue{150,0,0};
    const cv::Vec3b green{33, 176, 50};
    const cv::Vec3b brown{27, 76, 105};
    const cv::Vec3b yellow{39, 159, 186};
    const cv::Vec3b red{50, 50, 179};
    const cv::Vec3b white{255,255,255};
    const cv::Vec3b grey{196,196,196};

private:
    const cv::Vec3b col_index[DISCRETE_STEPS] = {
        black, white, red, yellow, green, blue, dark_blue, black
    };

public:
    explicit ColorizeWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::VEC_3_CHAR_8}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

protected:
    cv::Mat tmp;

    void start_up() override
    { /* No init required right now */ }

    void work() override
    {
        tmp = std::move(this->m_work_matrix);
        this->m_work_matrix = cv::Mat(tmp.size(), CV_8UC3);

        this->m_work_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos){
            pixel = col_index[tmp.at<uchar>(pos)];
        });
    }

};

};
