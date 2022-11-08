#include <opencv2/opencv.hpp>
#include <fstream>

int main(void)
{
    std::ofstream dlm_out("../DuneLogo.dres");
    dlm_out << "a";
    dlm_out.flush();
    dlm_out.close();
}
