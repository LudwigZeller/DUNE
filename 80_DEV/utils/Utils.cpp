//
// Created by Ludwig Zeller on 03.08.2022.
//
#include "Utils.hpp"

cv::Mat frame_to_mat(const rs2::frame &f) {
    using namespace cv;
    using namespace rs2;

    auto vf = f.as<video_frame>();
    const int w = vf.get_width();
    const int h = vf.get_height();

    if (f.get_profile().format() == RS2_FORMAT_BGR8) {
        return Mat{Size(w, h), CV_8UC3, (void *) f.get_data(), Mat::AUTO_STEP}.clone();
    } else if (f.get_profile().format() == RS2_FORMAT_RGB8) {
        auto r_rgb = Mat{Size(w, h), CV_8UC3, (void *) f.get_data(), Mat::AUTO_STEP};
        Mat r_bgr;
        cvtColor(r_rgb, r_bgr, COLOR_RGB2BGR);
        return r_bgr.clone();
    } else if (f.get_profile().format() == RS2_FORMAT_Z16) {
        return Mat{Size(w, h), CV_16UC1, (void *) f.get_data(), Mat::AUTO_STEP}.clone();
    } else if (f.get_profile().format() == RS2_FORMAT_Y8) {
        return Mat{Size(w, h), CV_8UC1, (void *) f.get_data(), Mat::AUTO_STEP}.clone();
    } else if (f.get_profile().format() == RS2_FORMAT_DISPARITY32) {
        return Mat{Size(w, h), CV_32FC1, (void *) f.get_data(), Mat::AUTO_STEP}.clone();
    }
    throw std::runtime_error("Frame format is not supported!");
}

cv::Mat dres_asset_to_color_mat(unsigned int width, unsigned int height, const char *data)
{
    // Allocated data init
    cv::Mat tmp = cv::Mat(cv::Size(width, height), CV_8UC3);
    uchar *dat = (uchar*) tmp.ptr<uchar[3]>();
    int x = 0,      //< X coordinate
        y = 0,      //< Y coordinate
        c = 0,      //< Repeat Counter
        n = 0;      //< R/G/B coordinate
    uchar dh = 0u,  //< High nibble
        dl = 0u;    //< Low nibble

    std::string last_color = "";

    unsigned long long until = strlen(data);

    #define _g dat[n + 3 * (x + y * width)]
    #define _h2i(val) ((val >= 'A' && val <= 'F') ? (val - 'A' + 10) : (val - '0'))
    #define _c2d(high, low) ((_h2i(high) << 4) | _h2i(low))

    for(unsigned long long i = 0ULL; i < until; i += 2)
    {
        if(data[i] >= 'a' && data[i] <= 'z')
        {
            last_color = "-";
            c = data[i] - 'a' + 2;
            dh = data[i+1];
            dl = data[i+2];
            i++;

            for(; c > 0; c--)
            {
                _g = _c2d(dh, dl);
                if(++n > 2)
                {
                    n = 0;
                    x++;
                }
            }
        }
        else
        {
            dh = data[i];
            dl = data[i+1];
            _g = _c2d(dh, dl);

            if(++n > 2)
            {
                n = 0;
                x++;
            }
        }

        if(x >= width)
        {
            x = 0;
            y++;
        }
    }

    return {tmp};
}

void capture_color(const cv::Mat &cln)
{
    static int check = 0;
    while(file_exists(std::string("capture-") + std::to_string(check) + ".dres")) check++;
    dres_save_color_mat(cln, std::string("CAPTURE_") + std::to_string(check), std::string("capture-") + std::to_string(check) + ".dres");
}

void capture_depth(const cv::Mat &cln)
{
    static int check = 0;
    while(file_exists(std::string("capture-depth-") + std::to_string(check) + ".dres")) check++;
    dres_save_depth_mat(cln, std::string("CAPTURE_DEPTH_") + std::to_string(check), std::string("capture-depth-") + std::to_string(check) + ".dres");
}

cv::Mat dres_asset_to_depth_mat(unsigned int width, unsigned int height, const short data[])
{
    cv::Mat tmp = cv::Mat(cv::Size(width, height), CV_16U);
    short *dat = (short*) tmp.ptr<short>();
    unsigned long long until = width * height;

    for(auto i = 0ULL; i < until; i++)
    {
        dat[i] = data[i];
    }

    return tmp;
}
