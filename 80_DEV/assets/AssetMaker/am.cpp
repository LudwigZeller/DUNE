#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>
#include "../../utils.hpp"

#define STRINGIZE(a) #a
#define XSTRINGIZE(a) STRINGIZE(a)

#define PATH_PNG XSTRINGIZE(_PATH_PNG)
#define RES_NAME XSTRINGIZE(_RES_NAME)
#define PATH_RES XSTRINGIZE(_PATH_RES)

int main(void)
{
    cv::Mat mat = cv::imread(PATH_PNG);
    saveMatAsFile(mat, RES_NAME, PATH_RES);
}
