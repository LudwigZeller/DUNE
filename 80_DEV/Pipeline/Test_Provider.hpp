//
// Created by eragon on 11/18/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
#define DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP

#include "Provider.hpp"
#include <chrono>
#include <thread>
#include <utility>

class Test_Provider : public Provider {
private:
    cv::Mat saved_mat;
public:
    explicit Test_Provider(std::string id, cv::Mat &dat) : Provider(std::move(id), MatIOType::SHORT_16)
    {
        saved_mat = std::move(dat);
    }

    void work() override
    {
        const static std::chrono::milliseconds dur{67};
        auto start = std::chrono::steady_clock::now();
        this->m_work_matrix = saved_mat.clone();
        while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start) < dur);
    }

    cv::Mat *get_test_adr()
    {
        return &saved_mat;
    }
};

#endif //DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
