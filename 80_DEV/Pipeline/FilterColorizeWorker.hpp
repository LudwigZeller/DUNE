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
    const cv::Vec3b ocean_blue{138,94,51};
    const cv::Vec3b shore_blue{179,136,73};
    const cv::Vec3b blue{255,0,0};
    const cv::Vec3b dark_blue{150,0,0};
    const cv::Vec3b plains_green{34,148,49};
    const cv::Vec3b forest_green{29,99,8};
    const cv::Vec3b limestone_yellow{94, 153, 163};
    const cv::Vec3b granite_orange{71,91,125};
    const cv::Vec3b snowy_white{189,189,160};
    const cv::Vec3b green{33, 176, 50};
    const cv::Vec3b brown{27, 76, 105};
    const cv::Vec3b yellow{39, 159, 186};
    const cv::Vec3b red{50, 50, 179};
    const cv::Vec3b white{255,255,255};
    const cv::Vec3b grey{196,196,196};

private:
    const cv::Vec3b col_index[DISCRETE_STEPS] = {
        //black, white, red, yellow, green, blue, dark_blue, black
        black,
        //Ocean has #1-#4
        ocean_blue, ocean_blue * 1.1, shore_blue, shore_blue * 1.2,
        //Green plane has #5-#8
        limestone_yellow * 1.5, plains_green * 1.1, forest_green, forest_green * 1.2,
        //Stone plane has #9-#12
        limestone_yellow, limestone_yellow * 1.1, granite_orange, granite_orange * 0.9,
        //Snow plane has #13-#14
        snowy_white, white,
        //
        black
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
            uchar &c = tmp.at<uchar>(pos);
            pixel = col_index[c & ~LINE_MASK];
            pixel *= 1.0 - 0.5 * ((LINE_MASK & c) > 0);
        });
    }

};

};
