#pragma once

#include "Worker.hpp"

const static cv::Vec3b black{0,0,0};
const static cv::Vec3b ocean_blue{138,94,51};
const static cv::Vec3b shore_blue{179,136,73};
const static cv::Vec3b blue{255,0,0};
const static cv::Vec3b dark_blue{150,0,0};
const static cv::Vec3b plains_green{34,148,49};
const static cv::Vec3b forest_green{29,99,8};
const static cv::Vec3b limestone_yellow{94, 153, 163};
const static cv::Vec3b granite_orange{71,91,125};
const static cv::Vec3b snowy_white{189,189,160};
const static cv::Vec3b green{33, 176, 50};
const static cv::Vec3b brown{27, 76, 105};
const static cv::Vec3b yellow{39, 159, 186};
const static cv::Vec3b red{50, 50, 179};
const static cv::Vec3b white{255,255,255};
const static cv::Vec3b grey{196,196,196};
const static cv::Vec3b purple{255,10,200};
const static cv::Vec3b orange{20,130,220};

const static cv::Vec3b col_index[DISCRETE_STEPS] = {
    //black, white, red, yellow, green, blue, dark_blue, black
    black,
    //Ocean has #1-#4
    ocean_blue, ocean_blue * 1.1, shore_blue, shore_blue * 1.2,
    //Green plane has #5-#8
    plains_green * 1.2, plains_green * 1.1, plains_green * 0.9, forest_green,
    //Stone plane has #9-#12
    limestone_yellow, limestone_yellow * 1.1, granite_orange, granite_orange * 0.9,
    //Snow plane has #13-#14
    snowy_white, white,
    //
    black
};

const static cv::Vec3b pride_index[DISCRETE_STEPS] = {
    black,
    red,red,red,
    orange, orange,
    yellow, yellow,
    green, green,
    blue, blue,
    purple, purple, purple,
    black
};

const static cv::Vec3b beach_index[DISCRETE_STEPS] = {
    //black, white, red, yellow, green, blue, dark_blue, black
    black,
    //Ocean has #1-#4
    ocean_blue, ocean_blue * 1.1, shore_blue, shore_blue * 1.2,
    //TODO: Is a joke for now
    black, black, black, black, black, black, black, black, black, black, black
};

const static cv::Vec3b difference_index[DISCRETE_STEPS] = {
    //black
    black,
    //below = red
    red, red * 0.8, red * 0.65, red * 0.5, red * 0.3, red * 0.2, red * 0.1,
    //above = green
    green * 0.1, green * 0.2, green * 0.3, green * 0.5, green * 0.65, green * 0.8, green,
    //black
    black
};

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
    enum COLORIZE_TYPE_e {
        DEFAULT = 0,
        PRIDE = 1,
        BEACH = 2,
        DIFFERENCE = 3,
    };

protected:
    COLORIZE_TYPE_e colorize_type = DEFAULT;
    cv::Vec3b const* index_ptr;

public:
    explicit ColorizeWorker(std::string id, COLORIZE_TYPE_e ctype = DEFAULT): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::VEC_3_CHAR_8}, colorize_type(ctype)
    {
        
        switch(ctype)
        {
            case BEACH:
                index_ptr = beach_index;
            break;
            case PRIDE:
                index_ptr = pride_index;
            break;
            case DIFFERENCE:
                index_ptr = difference_index;
            break;
            default:
                index_ptr = col_index;
            break;
        }
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
            pixel = index_ptr[c & ~LINE_MASK];
            pixel *= 1.0 - 0.5 * ((LINE_MASK & c) > 0);
        });
    }

};

};
