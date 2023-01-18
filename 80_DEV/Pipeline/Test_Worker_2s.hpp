//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_2S_HPP
#define DUNE_PIPELINE_TEST_WORKER_2S_HPP

#include "../Worker.hpp"
#include <chrono>
#include <thread>

class Test_Worker_2s : public Worker {
public:
    explicit Test_Worker_2s(std::string id) : Worker(std::move(id)) {
        clog(info) << get_id() << " initialized!" << std::endl;
    };

    void work() override {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2s);
        clog(info) << get_id() << " finished work!" << std::endl;
    };
};

#endif //DUNE_PIPELINE_TEST_WORKER_2S_HPP
