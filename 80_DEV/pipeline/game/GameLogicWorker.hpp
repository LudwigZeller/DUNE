#pragma once

#include "../Worker.hpp"
#include <list>

/**
 * @brief Project's Simulation namespace
*/
namespace Simulation {

namespace Game {
    enum class Team {
        Red,
        Blue
    };

    enum class FighterType {
        FootSoldier
    };

    struct Fighter { 
        int x; 
        int y; 
        Team team; 
        FighterType piece; 
        float lifepoints;
    };
};
/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class GameLogicWorker : public Worker
{
protected:
    //std::list<Game::Fighter> fighters;
    std::map<Game::Team, std::list<Game::Fighter>> fighters{};
    std::mutex fighter_mutex{};
    
public:
    
    void clear() {
        this->fighters.clear(); 
    } 

    void place(Game::Fighter fighter) {
        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        this->fighters[fighter.team].push_back(fighter);
    }

    std::list<Game::Fighter> get_pieces() {
        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        std::list<Game::Fighter> list{};

        for (auto & [team, fighter_team] : this->fighters)
        {
            list.insert(list.end(), fighter_team.begin(), fighter_team.end());
        }

        return list;
    }   

    GameLogicWorker(std::string id): Worker{std::move(id), MatIOType::CHAR_8, MatIOType::CHAR_8} {}

protected:
    void start_up() override{
    }

    void work() override
    {
        if(std::rand() % 10 == 0) {
            this->place(Game::Fighter{
                std::rand() % STREAM_WIDTH,
                std::rand() % STREAM_HEIGHT,
                std::rand() % 2 ? Game::Team::Blue : Game::Team::Red,
                Game::FighterType::FootSoldier,
                100.0
            });
        }

        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        static auto distance = [] (const Game::Fighter &a, const Game::Fighter &b) -> double {
            return 0.1 + std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
        };

        //auto &fighters_team = this->fighters[Game::Team::Blue];
        //auto team = Game::Team::Blue;
        //auto enemy = team == Game::Team::Blue ? Game::Team::Red : Game::Team::Blue;
        for (auto &[team, fighters_team] : this->fighters)
        {
            auto enemy = team == Game::Team::Blue ? Game::Team::Red : Game::Team::Blue;
            for (auto &fighter : fighters_team)
            {
                auto &nearest = this->fighters[enemy].front();
                double min = distance(nearest, fighter);
                for (Game::Fighter &enemy_fighter : this->fighters[enemy])
                {
                    double dist = distance(enemy_fighter, fighter);
                    if (dist < min) {
                        min = dist;
                        nearest = enemy_fighter;
                    }
                }

                if(min < 4) fighter.lifepoints--;
                else fighter.lifepoints = ___min_(100, fighter.lifepoints + 1);


                fighter.x -= (int)((double) (fighter.x - nearest.x) / min * 5.);
                fighter.y -= (int)((double) (fighter.y - nearest.y) / min * 5.);

                fighter.x = ___min_(CUTOFF_RIGHT - 20, ___max_(CUTOFF_LEFT + 20, fighter.x));
                fighter.y = ___min_(CUTOFF_BOT, ___max_(CUTOFF_TOP + 20, fighter.y));
            }

        }

        //for (auto &runner : this->fighters[enemy])
        {

        }

        for (auto & [team, fighter_team] : this->fighters) {
            fighter_team.remove_if([](auto fighter){
                if(fighter.lifepoints < 0.){
                    clog(info) << "Dead" << std::endl;
                    return true;
                }
                return false;
            });
        }
    }
};
};