//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_1MS_HPP
#define DUNE_PIPELINE_TEST_WORKER_1MS_HPP

#include "Worker.hpp"
#include <chrono>
#include <thread>
#include <utility>

class Test_Worker_1ms : public Worker{
public:
    Test_Worker_1ms(std::string id) : Worker(std::move(id)){
        clog(info) << "Test_Worker_1ms initialized!" << std::endl;
    };
    void work() override{
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        clog(info) << "Test_Worker_1ms finished work!" << std::endl;
    };
};


#endif //DUNE_PIPELINE_TEST_WORKER_1MS_HPP
