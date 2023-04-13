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
    std::list<Game::Fighter> fighters;
    std::mutex fighter_mutex{};
    
public:
    
    void clear() {
        this->fighters.clear(); 
    } 

    void place(Game::Fighter fighter) {
        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        this->fighters.push_back(fighter);
    }

    std::list<Game::Fighter> get_pieces() {
        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        return std::list<Game::Fighter>{fighters};
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
                Game::Team::Blue,
                Game::FighterType::FootSoldier,
                100.0
            });
        }
        std::lock_guard<std::mutex> lock(this->fighter_mutex);
        for (auto &fighter : this->fighters)
        {
            switch (std::rand() % 5) {
                case 0:
                    fighter.x += 1;
                    break;
                case 1:
                    fighter.x -= 1;
                    break;
                case 2:
                    fighter.y += 1;
                    break;
                case 3:
                    fighter.y -= 1;
                    break;
                case 4:
                    fighter.lifepoints -= 1.;
                    break;
            }          

        }

        fighters.remove_if([](auto fighter){
            if(fighter.lifepoints < 0.)
                return true;
            return false;
        });
    }
};
};