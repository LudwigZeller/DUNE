#include <opencv2/opencv.hpp>
#include "Window.hpp"
#include "config.hpp"

void test(Window &win)
{
    cv::Mat mog = cv::Mat::zeros(STREAM_HEIGHT, STREAM_WIDTH, CV_8UC3);

    std::chrono::milliseconds ms{100};
    auto start = std::chrono::steady_clock::now();

    int xi = 2, yi = 2;
    int dir = 1;

    win.render_matrix(std::move(mog));
    while(win) 
    {
        if(win.wpressed)
        {
            win.wpressed = false;
            return;
        }

        if(start + ms < std::chrono::steady_clock::now())
        {
            mog = cv::Mat::zeros(STREAM_HEIGHT, STREAM_WIDTH, CV_8UC3);
            mog(cv::Rect{
                (STREAM_WIDTH / 2) - 1,
                (STREAM_HEIGHT / 2) - 10,
                2, 21
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});
            mog(cv::Rect{
                (STREAM_WIDTH / 2) - 10,
                (STREAM_HEIGHT / 2) - 1,
                21, 2
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});

            if((yi += dir) >= STREAM_HEIGHT /2 || yi <= 2) dir = -dir;
            xi += dir;

            mog(cv::Rect{
                (STREAM_WIDTH / 2) - xi,
                (STREAM_HEIGHT / 2) - yi,
                1, 2*yi
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});
            mog(cv::Rect{
                (STREAM_WIDTH / 2) - xi,
                (STREAM_HEIGHT / 2) - yi,
                2*xi, 1
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});
            mog(cv::Rect{
                (STREAM_WIDTH / 2) + xi - 1,
                (STREAM_HEIGHT / 2) - yi,
                1, 2*yi
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});
            mog(cv::Rect{
                (STREAM_WIDTH / 2) - xi,
                (STREAM_HEIGHT / 2) + yi - 1,
                2*xi, 1
            }).forEach<cv::Vec3b>([](cv::Vec3b &p, const int *pos){p = {0,0,255};});
        win.render_matrix(std::move(mog));
        }
    }
}
