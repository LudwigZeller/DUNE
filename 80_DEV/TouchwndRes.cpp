#include "TouchwndRes.hpp"
#include "utils.hpp"

#include "staticassets/mc_prev.dres"
#include "staticassets/smooth_prev.dres"

void initMats()
{
    asset1 = assetToMat(MC_PREV_WIDTH, MC_PREV_HEIGHT, MC_PREV_DATA);
    cv::resize(asset1, asset1, cv::Size{800, 450});
    asset2 = assetToMat(SMOOTH_PREV_WIDTH, SMOOTH_PREV_HEIGHT, SMOOTH_PREV_DATA);
    cv::resize(asset2, asset2, cv::Size{800, 450});
    cv::putText(asset1, "Hier drücken", cv::Point2i{20, 20}, cv::FONT_HERSHEY_PLAIN, 6, cv::Scalar{1,1,1}, 1, 8, true);
    cv::putText(asset2, "Hier drücken", cv::Point2i{20, 20}, cv::FONT_HERSHEY_PLAIN, 6, cv::Scalar{1,1,1}, 1, 8, true);
}

cv::Mat getCaptureIndex(int index)
{
#define RTi(a,b) if(index == a) return b
    RTi(1,asset2);
    RTi(2,asset1);
    RTi(0,asset2);
#undef RTi
    return asset2;
}
