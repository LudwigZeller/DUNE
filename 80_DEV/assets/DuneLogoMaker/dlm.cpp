#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>

static char translate[17] = "0123456789ABCDEF";

#define PATH_PNG "../DuneLogo.png"
#define PATH_RES "../DuneLogo.dres"
#define RES_NAME "DLOGO"

int main(void)
{
    cv::Mat mat = cv::imread(PATH_PNG);
    uchar *dat = (uchar*) mat.ptr<uchar[3]>();
    cv::Size s = mat.size();
    std::ofstream dlm_out(PATH_RES);
    std::cout << "Creating asset: " RES_NAME << std::endl;
    
    dlm_out <<
        "/**\n * Auto-generated resource \"" PATH_RES "\" originating from \"" PATH_PNG         \
        "\"\n * Asset \"" RES_NAME "\" created for DUNE Project\n * @author David Schoosleitner"                       \
        "\n * (c) David Schoosleitner 2022\n */\n\n#pragma once\n\n/***** DATA *****/\n" << std::endl;
    dlm_out << "//! Matrix Width\n#define " RES_NAME "_WIDTH (" << s.width << "u)\n" << std::endl;
    dlm_out << "//! Matrix Height\n#define " RES_NAME "_HEIGHT (" << s.height << "u)\n" << std::endl;
    dlm_out << "//! Matrix Data: Hex format 0x[BBGGRR]0x[....\n#define " RES_NAME "_DATA ( \\" << std::endl;

    std::string tmpdat;

    {
        std::stringstream dlm_tmp1;

        std::string row = "";
        std::string set_full = "";
        std::string set = "";
        uchar g = 0;
        uchar l = 255;
        int count = 0;

        for(int i = 0; i < s.height; i++)
        {
            dlm_tmp1 << "  \"";
            row = "";
            g = 255;
            l = 255;
            count = 0;
            for(int j = 0; j < s.width; j++)
            {
                for(int k = 0; k < 3; k++)
                {
                    l = g;
                    g = (dat[3*(j + i * s.width) + k]);
                    if(g == l && count < 26)
                    {
                        set = (char)(count + 'a');
                        set += translate[g >> 4];
                        set += translate[g & 0x0F];
                        count++;
                    }
                    else
                    {
                        count = 0;
                        row += set;
                        set = translate[g >> 4];
                        set += translate[g & 0x0F];
                    }
                }
            }
            dlm_tmp1 << row << "\" \\" << std::endl;

        }
        tmpdat = dlm_tmp1.str();
    }

    dlm_out << tmpdat << std::flush;

    dlm_out << ")\n/// " RES_NAME "\n" << std::endl;
    dlm_out.close();
    std::cout << "Done " RES_NAME << std::endl;
    
}
