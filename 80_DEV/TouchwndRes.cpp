#include "TouchwndRes.hpp"
#include "utils.hpp"

#include "staticassets/mc_prev.dres"
#include "staticassets/smooth_prev.dres"
#include "staticassets/df_prev.dres"
#include "staticassets/stripe_prev.dres"

void text_util(cv::Mat &m, int x1, int y1, int x2, const char *text)
{
    int ypos = y1 + 10;
    std::string a{};
    int state = 0;
    char const *cur = text;

    while(*cur)
    {
        if(*cur == '\n')
        {
            cv::putText(m, a.c_str(), cv::Point2i{state ? x2 : x1, state ? ypos : y1}, cv::FONT_HERSHEY_PLAIN, 3 - state, cv::Scalar{1,1,1}, 4, 8, false);
            ypos += 40;
            state = 1;
            a.clear();
        }
        else
        {
            a += *cur;
        }
        cur++;
    }
    cv::putText(m, a.c_str(), cv::Point2i{state ? x2 : x1, state ? ypos : y1}, cv::FONT_HERSHEY_PLAIN, 3 - state, cv::Scalar{1,1,1}, 4, 8, false);
}

void initMats()
{
    asset_at_stripe = assetToMat(STRIPE_WIDTH, STRIPE_HEIGHT, STRIPE_DATA);
    cv::resize(asset_at_stripe, asset_at_stripe, cv::Size{800, 450});
    asset_at_minecraft = assetToMat(MC_PREV_WIDTH, MC_PREV_HEIGHT, MC_PREV_DATA);
    cv::resize(asset_at_minecraft, asset_at_minecraft, cv::Size{800, 450});
    asset_at_smooth = assetToMat(SMOOTH_PREV_WIDTH, SMOOTH_PREV_HEIGHT, SMOOTH_PREV_DATA);
    cv::resize(asset_at_smooth, asset_at_smooth, cv::Size{800, 450});
    asset_at_difference = assetToMat(DF_PREV_WIDTH, DF_PREV_HEIGHT, DF_PREV_DATA);
    cv::resize(asset_at_difference, asset_at_difference, cv::Size{800, 450});

    text_util(asset_at_difference, 80, 300, 90, "Jetzt: Unterschied\nFilter wechseln:\n-> zu Streifen ->\n<- zu Smooth <-");
    text_util(asset_at_minecraft, 80, 100, 90, "Jetzt: Blockcraft\nFilter wechseln:\n-> zu Smooth ->\n<- zu Streifen <-");
    text_util(asset_at_smooth, 80, 100, 90, "Jetzt: Smooth\nFilter wechseln:\n-> zu Unterschied ->\n<- zu Blockcraft <-"); 
    text_util(asset_at_stripe, 80, 100, 90, "Jetzt: Streifen\nFilter wechseln:\n-> zu Blockcraft ->\n<- zu Unterschied <-");
}

cv::Mat getCaptureIndex(int index)
{
#define RTi(a,b) if(index == a) return b
    RTi(0,asset_at_minecraft);
    RTi(1,asset_at_smooth);
    RTi(2,asset_at_difference);
    RTi(3,asset_at_stripe);
#undef RTi
    return asset_at_minecraft;
}
