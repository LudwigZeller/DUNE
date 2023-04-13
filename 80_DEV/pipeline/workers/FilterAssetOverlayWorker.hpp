#pragma once

#include "../Worker.hpp"

#include "../../assetRTE.hpp"

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

class AssetOverlayWorker : public Worker
{
private:
    Asset_Type asset_type;

public:
    AssetOverlayWorker(std::string id, Asset_Type _asset_type): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8},
        asset_type(_asset_type)
    { /* No further construction required */ }

protected:
    void start_up() override
    { /* nothing required */ }

    void work() override
    {
        static std::vector<aRTE_point_bool> &rpb = get_kernel(asset_type);
        for(int i = 0; i < rpb.size(); i++)
        {
            uchar c = this->m_work_matrix.at<uchar>(rpb[i].p);
            switch(asset_type)
            {
                case BC_Trees:
                    rpb[i].b = c > 4 && c < 9;
                break; case BE_Palms:
                    rpb[i].b = c > 4;
                break;
            }
        }
    }
};

};
