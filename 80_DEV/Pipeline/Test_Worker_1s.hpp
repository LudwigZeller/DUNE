//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_1S_HPP
#define DUNE_PIPELINE_TEST_WORKER_1S_HPP

#include "../Worker.hpp"
#include <chrono>
#include <thread>
#include <utility>

class Test_Worker_1s : public Worker {
public:
    explicit Test_Worker_1s(std::string id) : Worker(std::move(id)) {};

    void work() override {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

    };
};

#endif //DUNE_PIPELINE_TEST_WORKER_1S_HPP
