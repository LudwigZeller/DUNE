#pragma once

#include "Worker.hpp"

#include "../assets/Tree.dres"
#include "AssetRTE.hpp"

const cv::Vec3b bl{0,0,0};

std::vector<aRTE_point_bool> aRTE_fwmkernel{};

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class FunnyWorker : public Worker
{
private:
    cv::Mat tree;
public:
    FunnyWorker(std::string _id): Worker(std::move(_id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8)
    {
        clog(info) << "sussy baka" << std::endl;
        tree = assetToMat(tree_asmak_WIDTH, tree_asmak_HEIGHT, tree_asmak_DATA);
        for(int i = 0; i < D_aRTE_fwmkernelsize; i++)
        {
            int x = (std::rand() % (STREAM_WIDTH - 2*tree_asmak_WIDTH)) + tree_asmak_WIDTH;
            int y = (std::rand() % (STREAM_HEIGHT - 2*tree_asmak_HEIGHT)) + tree_asmak_HEIGHT;
            aRTE_fwmkernel.push_back(aRTE_point_bool{.r = cv::Rect{
                x, y,
                tree_asmak_WIDTH, tree_asmak_HEIGHT
            },
            .p = cv::Point{x + tree_asmak_WIDTH / 2, y + tree_asmak_HEIGHT / 2},
            .b = std::atomic_bool{false}});
        }

    }

protected:
    void start_up() override
    { /*eerereree*/}

    void work() override
    {
        for(aRTE_point_bool &r : aRTE_fwmkernel)
        {
            if(r.b)
                this->m_work_matrix(r.r).forEach<cv::Vec3b>([&](cv::Vec3b &p, const int *pos){
                    cv::Vec3b &r = this->tree.at<cv::Vec3b>(pos);
                    p = (r != bl) ? r : p;
                });
        }
    }
};

};
