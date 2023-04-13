#pragma once

#include "../Worker.hpp"
#include "../../calibRTE.hpp"

double __sw_cord_factor = 1.0 / std::sqrt((double) (STREAM_WIDTH * STREAM_WIDTH) * 0.25 + (double) (STREAM_HEIGHT * STREAM_HEIGHT) * 0.25);
double __sw_x_offset = -STREAM_WIDTH * 0.5;
double __sw_y_offset = -STREAM_HEIGHT * 0.5;
double __sw_disc_factor = 0.5 / (double) DISCRETE_STEPS;

inline double get_alt_at(const int *rpos, uchar &val)
{
    double rposx = __sw_cord_factor * ((double)rpos[1] + __sw_x_offset);
    double rposy = __sw_cord_factor * ((double)rpos[0] + __sw_y_offset);
    double raw_val = rposx * rposx + rposy * rposy;
    return raw_val * __sw_disc_factor * (double) val;
}

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ScaleWorker : public Worker
 * @brief Responsible for de-3d-izing
 */
class ScaleWorker : public Worker
{
private:
    std::vector<cv::Mat> m_mat_buf{DISCRETE_STEPS - 1};

public:
    ScaleWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8}
    { /* No extra construction required */ }

protected:
    void start_up() override
    { /* No start up required for now */ }

    void work() override
    {
        auto t = std::move(this->m_work_matrix);
        this->m_work_matrix = cv::Mat::zeros(t.size(), CV_8U);
        cv::Rect h{
            //(STREAM_WIDTH - scalar_kernel.width) / 2, (STREAM_HEIGHT - scalar_kernel.height) / 2,
            0, 0,
            scalar_kernel.width, scalar_kernel.height
        };
        cv::resize(t, this->m_work_matrix(h), scalar_kernel);
        /*for(cv::Mat &m : m_mat_buf)
        {
            m = cv::Mat::zeros(this->m_work_matrix.size(), CV_8U);
        }
        this->m_work_matrix.forEach<uchar>([&](uchar &c, const int *pos){
            for(uchar i = 0; i < c; i++)
                m_mat_buf[i].at<uchar>(pos) = 1;
            c = 0;
        });

        int idx = 0;
        for(cv::Mat &m : m_mat_buf)
        {
            cv::Mat _t = cv::Mat::zeros(this->m_work_matrix.size(), CV_8U);
            idx++;
            double scale = 1.0 - 0.004 * idx * idx;
            cv::resize(m, m, cv::Size{(int) (scale * _t.cols), (int) (scale * _t.rows)}, 0, 0, cv::INTER_NEAREST);
            m.copyTo(_t(cv::Rect(
                (int)(0.5 * _t.cols * (1.0 - scale)),
                (int)(0.5 * _t.rows * (1.0 - scale)),
                (int) (scale * _t.cols),
                (int) (scale * _t.rows)
            )));
            this->m_work_matrix.forEach<uchar>([&](uchar &d, const int *pos)
            {
                d += _t.at<uchar>(pos);
            });
        }*/
        /*this->m_tmp.forEach<uchar>([&](uchar &orig_pixel, const int *pos){
            double raw_val = get_alt_at(pos, orig_pixel);
            //uchar raw_amp = 1.0 / raw_val;
            this->m_work_matrix.at<uchar>(pos) -= 1;

            int cdiff[2] = {(int)((-pos[0] - __sw_y_offset) * raw_val) + pos[0], (int)((-pos[1] - __sw_x_offset) * raw_val) + pos[1]};
            this->m_work_matrix.at<uchar>(cdiff) += 1;
        });*/
    }
};

};
