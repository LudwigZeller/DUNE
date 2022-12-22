#pragma once

#include "Worker.hpp"
#include "CalibRTE.hpp"

class TranslatorWorker : public Worker
{
private:
    //cv::Mat m_tmp;
    const bool ptf;

public:
    explicit TranslatorWorker(std::string id, const bool ptf): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8), ptf(ptf)
    {
        clog(info) << id << " initialized!" << std::endl;
    }

protected:
    inline friend cv::Point2i operator-(const cv::Point2i &c) {
        return {-c.x, -c.y};
    }

    inline void start_up() override
    { /* none required */ 
        
    }

    inline void work() override
    {
        meta_translate(this->m_work_matrix, translation_vec);
        /*bool tx_sm_0 = 0 > -translation_vec.x;
        bool ty_sm_0 = 0 > -translation_vec.y;

        int x_area_cp = STREAM_WIDTH + (tx_sm_0 ? -translation_vec.x : (translation_vec.x));
        int y_area_cp = STREAM_HEIGHT + (ty_sm_0 ? -translation_vec.y : (translation_vec.y));

        cv::Rect src{
            !tx_sm_0 ? 0 : translation_vec.x,
            !ty_sm_0 ? 0 : translation_vec.y,
            x_area_cp, y_area_cp
        };

        cv::Rect dst{
            !tx_sm_0 ? -translation_vec.x : 0,
            !ty_sm_0 ? -translation_vec.y : 0,
            x_area_cp, y_area_cp
        };

        this->m_tmp = std::move(this->m_work_matrix);
        this->m_work_matrix = cv::Mat::zeros(this->m_tmp.size(), CV_8U);
        cv::copyTo(this->m_tmp(src), this->m_work_matrix(dst), cv::Mat());*/
    }
};
