#pragma once

#include "Worker.hpp"
#include "CalibRTE.hpp"

#define AMOUNT_CALIB_CTR_LOOPS 2
#define TIME_CALIB_CTR 60
#define TIME_CALIB_CTR_OFFS 30

class CalibWorker : public Worker
{
private:
    int xstart = 0, ystart = 0, xend = STREAM_WIDTH - 1, yend = STREAM_HEIGHT - 1;
    cv::Mat m_comparison;
    int tick_ct = TIME_CALIB_CTR_OFFS + TIME_CALIB_CTR * AMOUNT_CALIB_CTR_LOOPS;
    bool val_lp = true;
    int t_xi = 0;
    std::vector<cv::Point2i> m_point_cache[AMOUNT_CALIB_CTR_LOOPS] = {};
    cv::Point2i difference_pt[AMOUNT_CALIB_CTR_LOOPS];
    cv::Point2i pts_i[AMOUNT_CALIB_CTR_LOOPS] = {{400, 200}, {STREAM_WIDTH-400, STREAM_HEIGHT-200}};

public:
    explicit CalibWorker(std::string id): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::VEC_3_CHAR_8)
    {
        //cv::Point2i npts_i[AMOUNT_CALIB_CTR_LOOPS * 10];

        for(cv::Point2i &pt : difference_pt)
        {
            pt = {0,0};
        }
        /*for(int i = 0; i < AMOUNT_CALIB_CTR_LOOPS; i++)
        {
            difference_pt[i] = {0,0};
        }*/

        /*for(int i = 0; i < AMOUNT_CALIB_CTR_LOOPS * 10; i++)
        {
            int x = std::rand() % (STREAM_WIDTH - 800);
            int y = std::rand() % (STREAM_HEIGHT - 400);
            npts_i[i] = {x + 400, y + 200};
        }

        int sq_dist = -1;
        int sq_dist_n = -1;

        for(int i = 0; i < AMOUNT_CALIB_CTR_LOOPS * 10; i++)
        {
            for(int j = 0; j < AMOUNT_CALIB_CTR_LOOPS * 10; j++)
            {
                sq_dist_n
                    = (npts_i[i].x - npts_i[j].x)
                    * (npts_i[i].x - npts_i[j].x)
                    + (npts_i[i].y - npts_i[j].y)
                    * (npts_i[i].y - npts_i[j].y);
                    
                if(sq_dist_n > sq_dist)
                {
                    sq_dist = sq_dist_n;
                    pts_i[0] = {npts_i[i]};
                    pts_i[1] = {npts_i[j]};
                }
            }
        }*/
        clog(info) << id << " initialized!" << std::endl;
    }

protected:
    void start_up() override
    { /* None req */ }

    void work() override
    {
        val_lp = false;
        if(tick_ct > TIME_CALIB_CTR_OFFS)
        {
            int index = (tick_ct - TIME_CALIB_CTR_OFFS - 1) / TIME_CALIB_CTR;
            std::vector<cv::Point2i> pv_tmp;
            tick_ct--;
            this->m_work_matrix = cv::Mat::zeros({STREAM_WIDTH, STREAM_HEIGHT}, CV_8UC3);
            this->m_work_matrix(cv::Rect{
                pts_i[index].x - 5, pts_i[index].y - 5, 11, 11
            })
            .forEach<cv::Vec3b>([](cv::Vec3b &p, [[maybe_unused]] const int *pos){
                p = {255,0,0};
            });

            cv::Mat cmc = color_mat.clone();
            std::mutex mtx;
            cmc(cv::Rect{pts_i[index].x - 150, pts_i[index].y - 150, 301, 301})
            .forEach<cv::Vec3b>([&](cv::Vec3b &p, const int *pos){
                if(p[0] > 155 && p[2] < 150) {
                    std::lock_guard<std::mutex> lg{mtx};
                    pv_tmp.push_back(cv::Point2i{pos[1], pos[0]});
                }
            });
            int a = 0, b = 0, c = (int)pv_tmp.size();
            if(c <= 0) return;
            for(cv::Point2i p : pv_tmp)
            {
                a += p.x;
                b += p.y;
            }
            pv_tmp.clear();
            a /= c;
            b /= c;
            a += pts_i[index].x - 200;
            b += pts_i[index].y - 200;

            this->m_work_matrix(cv::Rect{
                a - 5 + difference_pt[index].x, b - 5 + difference_pt[index].y, 11, 11
            })
            .forEach<cv::Vec3b>([&](cv::Vec3b &p, const int *pos){
                p[2] = 255;
            });

            difference_pt[index] += {pts_i[index].x - a, pts_i[index].y - b};
            difference_pt[index] /= 2;
            m_point_cache[index].push_back({difference_pt[index]});
            
            return;
        }

        //!! Select points furthest from each other
        if(tick_ct == TIME_CALIB_CTR_OFFS)
        {
            for(int i = 0; i < AMOUNT_CALIB_CTR_LOOPS; i++)
            {
                difference_pt[i] = {0,0};
                std::size_t size = m_point_cache[i].size() / 2;

                for(std::size_t j = 0; j < size; j++)
                {
                    difference_pt[i] += m_point_cache[i].back();
                    m_point_cache[i].pop_back();
                }

                difference_pt[i] /= (int) size;
            }


            clog(warn) << "PT1: x" << pts_i[0].x << " y" << pts_i[0].y << "\n"
            << "CT1: x" << pts_i[0].x - difference_pt[0].x << " y" << pts_i[0].y - difference_pt[0].y << "\n"
            << "PT2: x" << pts_i[1].x << " y" << pts_i[1].y << "\n"
            << "CT2: x" << pts_i[1].x - difference_pt[1].x << " y" << pts_i[1].y - difference_pt[1].y << std::endl;

            double kx = (double) (pts_i[0].x - pts_i[1].x) / (double) (pts_i[0].x - pts_i[1].x - difference_pt[0].x + difference_pt[1].x);
            double ky = (double) (pts_i[0].y - pts_i[1].y) / (double) (pts_i[0].y - pts_i[1].y - difference_pt[0].y + difference_pt[1].y);

            //kx = (kx + ky) * 0.5;w
            //ky = kx;

            scalar_kernel = {(int)(kx * STREAM_WIDTH), (int)(ky * STREAM_HEIGHT)};
            translation_vec = {
                pts_i[0].x - (int)(kx * (pts_i[0].x - difference_pt[0].x)),
                pts_i[0].y - (int)(ky * (pts_i[0].y - difference_pt[0].y))
            };

            clog(err) << "Translation = " << translation_vec << std::endl;

            clog(err) << "Kx = " << kx << "\nKy = " << ky << std::endl;
        }

        if(tick_ct > 0)
        {
            tick_ct--;
            m_comparison = m_work_matrix;
            this->m_work_matrix = cv::Mat::zeros({STREAM_WIDTH, STREAM_HEIGHT}, CV_8UC3);
            return;
        }

        m_work_matrix -= m_comparison;

        for(int y = STREAM_HEIGHT - 200; y >= 100; y--) for(int x = 400; x < STREAM_WIDTH - 400; x++)
        {
            if(this->m_work_matrix.at<uchar>(y, x) > 6)
            {
                xstart = x;
                ystart = y;
                y = 0;
                val_lp = true;
                break;
            }
        }

        this->m_work_matrix = cv::Mat::zeros({STREAM_WIDTH, STREAM_HEIGHT}, CV_8UC3);
        this->m_work_matrix(cv::Rect{xstart, 0, 1, STREAM_HEIGHT})
        .forEach<cv::Vec3b>([](cv::Vec3b &p, [[maybe_unused]] const int *pos){
            p = {0,0,255};
        });
        this->m_work_matrix(cv::Rect{0, ystart, STREAM_WIDTH, 1})
        .forEach<cv::Vec3b>([](cv::Vec3b &p, [[maybe_unused]] const int *pos){
            p = {0,0,255};
        });

        if(val_lp)
        {
            if((t_xi += 20) > 400) t_xi = 0;
            this->m_work_matrix(cv::Rect{
                xstart - t_xi - 10,
                ystart - 10,
                21, 21 
            })
            .forEach<cv::Vec3b>([](cv::Vec3b &p, [[maybe_unused]] const int *pos){
                p = {255,255,255};
            });
        }

        cv::copyTo(color_mat(cv::Rect{
            STREAM_WIDTH / 4,
            STREAM_HEIGHT / 4,
            STREAM_WIDTH / 2,
            STREAM_HEIGHT / 2
        }), this->m_work_matrix(
            cv::Rect{
                STREAM_WIDTH / 2 - 1,
                STREAM_HEIGHT / 2 - 1,
                STREAM_WIDTH / 2,
                STREAM_HEIGHT / 2
            }
        ), cv::Mat());
    }
};
