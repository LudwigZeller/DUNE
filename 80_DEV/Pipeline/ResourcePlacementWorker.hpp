#pragma once

#include "Worker.hpp"

#include "../assets.dres"
#include "AssetRTE.hpp"

const cv::Vec3b bl{0,0,0};

/**
 * @brief Project's Filter namespace
*/
__ARTE_INIT_ namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class ResourcePlacementWorker : public Worker
{
private:
    cv::Mat tree;
    Asset_Type asset_type;

public:
    ResourcePlacementWorker(std::string _id, const Asset_Type _asset_type,
        const int width, const int height, const char *data) :
        Worker(std::move(_id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8),
        asset_type(_asset_type)
    {    
        tree = assetToMat(width, height, data);

        for(int i = 0; i < get_kernel(_asset_type).size(); i++)
        {
            int x = (std::rand() % (STREAM_WIDTH - 2*width)) + width;
            int y = (std::rand() % (STREAM_HEIGHT - 2*height)) + height;
            get_kernel(_asset_type)[i] = aRTE_point_bool{.r = cv::Rect{
                x, y,
                width, height
            },
            .p = cv::Point{x + (signed) width / 2, y + (signed) height / 2},
            .b = std::atomic_bool{false}};
        }

    }

protected:
    void start_up() override
    { /*eerereree*/}

    void work() override
    {
        for(aRTE_point_bool &r : get_kernel(asset_type))
        {
            if(r.b)
                this->m_work_matrix(r.r).forEach<cv::Vec3b>([&](cv::Vec3b &p, const int *pos)
                {
                    cv::Vec3b &r = this->tree.at<cv::Vec3b>(pos);
                    p = (r != bl) ? r : p;
                });
        }
    }
};

};
