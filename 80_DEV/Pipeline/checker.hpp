#pragma once

#include "Worker.hpp"

#include "AssetRTE.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

class checker : public Worker
{
private:
/*a*/
public:
    checker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    {
        clog(info) << "Xena" << std::endl;
    }

protected:
    void start_up() override
    { /* nothing required */ }

    void work() override
    {
        for(int i = 0; i < D_aRTE_fwmkernelsize; i++)
        {
            uchar c = this->m_work_matrix.at<uchar>(aRTE_fwmkernel[i].p);
            aRTE_fwmkernel[i].b = c > 5 && c < 9;
        }
    }
};

};
