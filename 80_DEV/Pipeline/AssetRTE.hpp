#pragma once
#include <opencv2/opencv.hpp>

enum Asset_Type
{   
    BC_Trees = 0,
    Palms = 1,
};

typedef struct aRTE_point_bool
{
    cv::Rect r;
    cv::Point p;
    bool b;
} aRTE_point_bool;

extern std::vector<aRTE_point_bool> aRTE_bc_trees;
extern std::vector<aRTE_point_bool> aRTE_palms;
extern unsigned int aRTE_difference_sum;

#define __ARTE_INIT_ \
std::vector<aRTE_point_bool> aRTE_bc_trees{20}; \
std::vector<aRTE_point_bool> aRTE_palms{50}; \
unsigned int aRTE_difference_sum = 0;

inline std::vector<aRTE_point_bool> &get_kernel(const Asset_Type type)
{
    switch(type)
    {
        case BC_Trees:
            return aRTE_bc_trees;
        case Palms:
            return aRTE_palms;
    }
    return aRTE_bc_trees;
}
