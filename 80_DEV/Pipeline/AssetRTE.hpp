#pragma once
#include <opencv2/opencv.hpp>

typedef struct aRTE_point_bool
{
    cv::Rect r;
    cv::Point p;
    bool b;
    //aRTE_point_bool &operator=(aRTE_point_bool &b) {this->r = b.r; this->p = b.p; this->b.store(b.b.load()); return *this;}
} aRTE_point_bool;

extern std::vector<aRTE_point_bool> aRTE_fwmkernel;
extern cv::Mat aRTE_fwmdat;
extern cv::Mat aRTE_fwmmask;
#define D_aRTE_fwmkernelsize 20
