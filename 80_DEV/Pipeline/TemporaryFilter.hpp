#pragma once

#include "Worker.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

class TmpFilter : public Worker
{
private:
    cv::Mat tmp;

    int capture_l = 0;
    std::string d_name;

    void capture()
    {
        capture_l = 15;

        std::string s = "mkdir ";
        d_name = std::to_string(std::rand() % 100000);
        s += d_name;
        (void)system(s.c_str());
    }

public:
    TmpFilter(std::string id):
        Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8}
    { /* No further construction required */ }

    void bind_to_window(Window &w)
    {
        w.bind_capture_adress([this](){this->capture();});
    }


protected:
    void start_up() override
    { /* nothing required */ }

    void work() override
    {
        if(capture_l > 0)
        {
            capture_l--;

            std::string s = std::to_string(14 - capture_l);
            s += ".jpg";

            cv::imwrite((d_name + "/" + s), m_work_matrix);

            if(capture_l == 0)
            {
                std::string l_cmd =
                    "ffmpeg -f image2 -i ";
                l_cmd += d_name + "/%d.jpg -framerate 15 -loop 0 -pix_fmt rgb24 -vf scale=\"720:480\" -q:v 2 " + d_name + ".gif";
                std::string m_cmd = "rm -r " + d_name + "/";
                (void)system(l_cmd.c_str());
                (void)system(m_cmd.c_str());
            }
        }
    }
};

};
