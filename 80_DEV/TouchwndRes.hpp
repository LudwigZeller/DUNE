#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

inline static cv::Mat asset_at_minecraft, asset_at_smooth, asset_at_difference, asset_at_stripe;

void initMats();
cv::Mat getCaptureIndex(int index);
