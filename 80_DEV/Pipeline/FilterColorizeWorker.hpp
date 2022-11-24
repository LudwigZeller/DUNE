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

    explicit ColorizeWorker(std::string id): Worker{std::move(id)}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

protected:

    void start_up() override
    { /* No init required right now */ }

    void work() override
    {
        cv::Mat outp(this->m_work_matrix.size(), CV_8UC3);
        this->m_work_matrix.forEach<double>([&](double &depth, const int *pos)
        {
            cv::Vec3b &pixel = outp.at<cv::Vec3b>(pos);
            if(depth < 0.2)
            {
                int hm = (((pos[0] % 192) / 64) + ((pos[1] % 192) / 64)) % 3;
                pixel = hm == 0 ? red : hm == 1 ? green : blue;
                return;
            }
            if(depth < 1 || depth > 1.24)
            {
                pixel = black;
            }
            else
            {
                int grad = ((depth - 1.0) / 0.24) * 6;
                pixel =
                    (0 == grad) ? white :
                    (1 == grad) ? red   :
                    (2 == grad) ? yellow:
                    (3 == grad) ? green :
                    (4 == grad) ? blue  :
                    dark_blue;
            }
        });
        this->m_work_matrix = outp;
    }

};

};
