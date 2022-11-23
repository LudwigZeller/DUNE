#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>
#include "../../utils.hpp"

#define PATH_PNG "../DuneLogo.png"
#define PATH_RES "../DuneLogo.dres"
#define RES_NAME "DLOGO"

int main(void)
{
    cv::Mat mat = cv::imread(PATH_PNG);
    saveMatAsFile(mat, RES_NAME, PATH_RES);
}
