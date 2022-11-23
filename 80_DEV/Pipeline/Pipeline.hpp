//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_PIPELINE_H
#define DUNE_PIPELINE_PIPELINE_H

#include <list>
#include <mutex>
#include <opencv2/opencv.hpp>

#include "Worker.hpp"
#include "Provider.hpp"

class Pipeline {
private:
    Provider *m_provider{};
    std::list<Worker *> m_pipeline{};
    std::mutex m_pipeline_mutex{};
    std::thread m_thread{};
    std::atomic_bool m_running = false;

public:
    explicit Pipeline(Provider *provider);

    ~Pipeline();

    bool start();

    bool stop();

    void push_worker(Worker *worker);

    Provider *replace_provider(Provider *provider);

    void insert_worker_before(std::string id, Worker *worker);

    Worker *remove_worker(std::string id);

    std::vector<std::string> get_pipeline();
};


#endif //DUNE_PIPELINE_PIPELINE_H
