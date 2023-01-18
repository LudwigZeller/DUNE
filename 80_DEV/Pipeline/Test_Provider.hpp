//
// Created by eragon on 11/18/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
#define DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP

#include "Provider.hpp"
#include <chrono>
#include <thread>
#include <utility>

#include "../../capture-depth-2.dres"

class Test_Provider : public Provider {
private:
    short *m_cursed_dat;
public:
    explicit Test_Provider(std::string id) : Provider(std::move(id)) {
        short cursed_temp[] = CAPTURE_DEPTH_2_DATA;
        m_cursed_dat = new short[CAPTURE_DEPTH_2_WIDTH * CAPTURE_DEPTH_2_HEIGHT];
        memcpy(m_cursed_dat, cursed_temp, CAPTURE_DEPTH_2_WIDTH * CAPTURE_DEPTH_2_HEIGHT);
    };

    ~Test_Provider()
    {
        delete[] m_cursed_dat;
    }

    void work() override {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(66ms);
        //m_work_matrix = std::move(assetToMat(CAPTURE_0_WIDTH,CAPTURE_0_HEIGHT,CAPTURE_0_DATA));
        m_work_matrix = std::move(assetToMatDepth(CAPTURE_DEPTH_2_WIDTH, CAPTURE_DEPTH_2_HEIGHT, m_cursed_dat));
        macro_matrixToMeters(m_work_matrix);
        //clog(info) << get_id() <<" provided Matrix!" << std::endl;
    };
};

#endif //DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
