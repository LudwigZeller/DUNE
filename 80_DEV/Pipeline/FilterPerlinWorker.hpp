#pragma once

#include "Worker.hpp"
#include "../PerlinNoise.hpp"
/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class DiscreticiserWorker : public Worker
 * @brief Discretization of meter data. 
*/
class PerlinWorker : public Worker
{
protected:
    cv::Mat m_perlin;
    int linger_tick;

public:
    explicit PerlinWorker(std::string id): Worker(std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8)
    { /* No extra construction required */ }

protected:
    void start_up() override
    {
        linger_tick = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
        m_perlin = cv::Mat{cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_16U};
        const short m_disc_start = 1150;
        const short m_disc_end = 1350;
        const short m_lin_steps = DISCRETE_STEPS;

        siv::PerlinNoise::seed_type seed = (unsigned) std::rand();
        siv::PerlinNoise perlin{seed};

        /*const double downscale_filter[16] = {
            1.00, 1.000, 0.999, 0.998,
            0.996, 0.993, 0.995, 0.999,
            0.997, 0.995, 0.995, 0.998,
            0.9991, 0.9992, 0.9993, 1.00
        };*/

        const double downscale_filter[16] = {
            1.00, 1.00, 0.99, 0.99,
            0.99, 0.99, 0.99, 0.99,
            0.99, 0.991, 0.991, 0.991,
            0.992, 0.992, 0.992, 0.992
        };
        
        cv::Mat floatpix{cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_32F};
        floatpix.forEach<float>([&](float &pixel, const int *pos)
        {
            float val = perlin.octave2D_01(0.002 * pos[0], 0.002 * pos[1], 4);
            pixel = val * 16.0 + 1;
        });

        double integr_vol = 0.0;
        for(int y = 0; y < STREAM_HEIGHT; y++)
            for(int x = 0; x < STREAM_WIDTH; x++)
            {
                integr_vol += floatpix.at<float>(y,x);
                m_perlin.at<short>(y,x) = (short) floatpix.at<float>(y,x);
            }

        while(integr_vol < TARGET_VOLUME_LOWER || integr_vol > TARGET_VOLUME_UPPER)
        {
            bool b = integr_vol < TARGET_VOLUME_LOWER;
            integr_vol = 0.0;

            for(int y = 0; y < STREAM_HEIGHT; y++)
                for(int x = 0; x < STREAM_WIDTH; x++)
                {
                    float &val = floatpix.at<float>(y,x);
                    val = b ? (val / downscale_filter[(int) val]) : (val * downscale_filter[(int) val]);
                    integr_vol += m_perlin.at<short>(y,x) = (short) val;
                }
            
            clog(warn) << "integr_vol: " << (int) integr_vol << " Target: " << TARGET_VOLUME << std::endl;
        }

        m_perlin.convertTo(m_perlin, CV_16U, -13.3333333, 1350);
    }

    void work() override
    {
        if(linger_tick)
        {
            linger_tick--;
            this->m_work_matrix = m_perlin.clone();
        }   
    }
};

};
