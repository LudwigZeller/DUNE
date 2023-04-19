#pragma once

#include "Worker.hpp"
#include "GameLogicWorker.hpp"
#include <vector>
#include <opencv2/opencv.hpp>

/**
 * @brief Project's Simulation namespace
*/
namespace Simulation {
/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class GameDrawWorker : public Worker
{
protected:
    Simulation::GameLogicWorker  *logic_worker;

public:
    GameDrawWorker(std::string id, Simulation::GameLogicWorker * game_logic_worker): 
    Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8},
    logic_worker(game_logic_worker)
    {}

protected:
    void start_up() override
    {
    }

    void work() override
    {
        std::list<Game::Fighter> fighters = this->logic_worker->get_pieces();
        for (Game::Fighter &fighter : fighters)
        {
            //cv::circle(m_work_matrix, cv::Point{ fighter.x,  fighter.y}, 5, cv::Scalar{0,fighter.lifepoints / 100. * 255,(1 - fighter.lifepoints / 100.) * 255}, -1);
            cv::Point center{fighter.x, fighter.y};
            double re, im;
            double deg_inc = 3.6;

            cv::circle(m_work_matrix, center, 7, cv::Scalar{127,127,127}, -1);

            cv::ellipse(m_work_matrix, 
                        center,
                        cv::Size{5,5},
                        0, // Direction
                        0, 360. * fighter.lifepoints / 100.,
                        cv::Scalar{0,fighter.lifepoints / 100. * 255,(1 - fighter.lifepoints / 100.) * 255},
                        -1);

                        

            //for (double deg = deg_inc; deg < fighter.lifepoints / 100. * 360.; deg+=deg_inc)
            //{
            //    sincos(deg, &im, &re);
            //    cv::Point stop{fighter.x + (int)(re * 5.), fighter.y + (int)(im * 5.)};
            //    cv::line(m_work_matrix, start, stop, cv::Scalar{0,fighter.lifepoints / 100. * 255,(1 - fighter.lifepoints / 100.) * 255});
            //    start = stop;
            //}

            
            switch (fighter.team) {
                case Game::Team::Blue:    
                    cv::circle(m_work_matrix, center, 3, cv::Scalar{255,0,0}, -1);
                    break;
                case Game::Team::Red:
                    cv::circle(m_work_matrix, center, 3, cv::Scalar{0,0,255}, -1);
                    break;
            }
            
        }
        
    }
};

};
