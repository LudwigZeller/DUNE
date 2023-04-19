#pragma once

#include "Worker.hpp"
#include "Provider.hpp"
#include "VisualCutWorker.hpp"
#include "Translator.hpp"
#include "FilterScaleWorker.hpp"
#include "FilterDiscreticiser.hpp"
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
    bool is_generator;

public:
    const float downscale_filter[DISCRETE_STEPS] =
    {
        1.000f, 1.000f, 0.990f, 0.990f,
        0.990f, 0.990f, 0.990f, 0.990f,
        0.990f, 0.991f, 0.991f, 0.991f,
        0.992f, 0.992f, 0.992f, 0.992f
    };

    const static short m_disc_start = 1150;
    const static short m_disc_end = 1350;
    const static short m_lin_steps = DISCRETE_STEPS;

    explicit PerlinWorker(std::string id, bool _is_generator = false): Worker(std::move(id), MatIOType::SHORT_16, MatIOType::SHORT_16), is_generator(_is_generator)
    { /* No further construction required -> Generation is in startup */ }

    cv::Mat get_matrix() const
    {
        return m_perlin;
    }

    void downscale()
    {
        if(!this->is_generator) return;
        m_perlin.forEach<float>([&](float &pixel, const int *pos)
        {
            pixel *= downscale_filter[(int) pixel];
        });
    }

    static void generate_perlin(cv::Mat &ref)
    {
        //! Specified generator worker
        PerlinWorker perlin_generator{"Generator_Perlin_Worker", true};
        //!! Transformation and cut workers
        DiscreticiserWorker discreticiser_generator{"Generator_Discreticiser_Worker"};
        ScaleWorker scale_generator{"Generator_Scale_Worker"};
        TranslatorWorker translator_generator{"Generator_Translator_Worker"};
        VisualCutWorker cut_generator{"Generator_Cut_Worker"};

        Worker *generator_queue[] = {&perlin_generator, &discreticiser_generator, &scale_generator,
            &translator_generator, &cut_generator};
        for(int i = 0; i < 5; i++) generator_queue[i]->start();

        //!! First-pass pipeline iteration
        cv::Mat pass;
        while(pass.empty())
        {
            perlin_generator.push(cv::Mat{});
            for (int i = 0; i < 5; i++)
            {
                if (!pass.empty()) generator_queue[i]->push(std::move(pass));
                pass = std::move(generator_queue[i]->pop());
            }
        }

        //!! First-pass volume elaboration
        unsigned int integr_vol = 0u;
        for(int y = 0; y < STREAM_HEIGHT; y++)
            for(int x = 0; x < STREAM_WIDTH; x++)
            {
                integr_vol += pass.at<char>(y,x);
            }
        clog(warn) << "new integr_vol: " << (int) integr_vol << ", with Target: " <<
            TARGET_VOLUME << std::endl;

        while(integr_vol > TARGET_VOLUME_UPPER)
        {
            //!! Reset and downscaling
            integr_vol = 0u;
            perlin_generator.downscale();
            
            //!! Cyclic pipeline iteration
            pass = cv::Mat{};
            while(pass.empty())
            {
                perlin_generator.push(cv::Mat{});
                for (int i = 0; i < 5; i++)
                {
                    if (!pass.empty()) generator_queue[i]->push(std::move(pass));
                    pass = std::move(generator_queue[i]->pop());
                }
            }

            //!! Cyclic volume elaboration
            for(int y = 0; y < STREAM_HEIGHT; y++)
                for(int x = 0; x < STREAM_WIDTH; x++)
                {
                    integr_vol += pass.at<char>(y,x);
                }
            
            clog(warn) << "new integr_vol: " << (int) integr_vol << ", with Target: " <<
                TARGET_VOLUME << std::endl;
        }

        pass = perlin_generator.get_matrix();
        pass.convertTo(ref, CV_16U, ((double) m_disc_start - (double) m_disc_end) /
            (double) (m_lin_steps - 1), m_disc_end);

        for(int i = 0; i < 5; i++) generator_queue[i]->stop();
    }

protected:
    void start_up() override
    {
        linger_tick = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
        if(is_generator)
        {
            m_perlin = cv::Mat{cv::Size{STREAM_WIDTH, STREAM_HEIGHT}, CV_32F};

            siv::PerlinNoise::seed_type seed = (unsigned) std::rand();
            siv::PerlinNoise perlin{seed};
            
            m_perlin.forEach<float>([&](float &pixel, const int *pos)
            {
                float val = perlin.octave2D_01(0.002 * pos[0], 0.002 * pos[1], 4);
                pixel = val * 16.0 + 1;
            });
        }
        else
        {
            generate_perlin(this->m_perlin);
        }
    }

    void work() override
    {
        if(linger_tick || this->is_generator)
        {
            linger_tick--;
            if(!this->is_generator)
                this->m_work_matrix = m_perlin.clone();
            else
                m_perlin.convertTo(this->m_work_matrix, CV_16U,
                    ((double) m_disc_start - (double) m_disc_end) /
                    (double) (m_lin_steps - 1), m_disc_end);
        }   
    }
};

};
