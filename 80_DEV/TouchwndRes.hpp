#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

inline static cv::Mat asset1, asset2;

void initMats();
cv::Mat getCaptureIndex(int index);
