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

/**
 * The Heart of the Project
 * Enables fast concurrent Matrix calculations
 */
class Pipeline {
private:
    Provider *m_provider{};
    std::list<Worker *> m_pipeline{};
    std::mutex m_pipeline_mutex{};
    std::thread m_thread{};
    std::atomic_bool m_running = false;

public:
    /**
     * Constructs a Pipeline and binds it to a provider
     * @param provider Provider to bind to
     */
    explicit Pipeline(Provider *provider);

    ~Pipeline();

    /**
     * Starts Provider and every Worker in Pipeline.
     * Spawns a thread polling workers and transporting matrices through the pipeline
     * @return If the pipeline was already running
     */
    bool start();

    /**
     * Stops Provider and every Worker in Pipeline.
     * Joins the polling and transporting thread.
     * @return If the Pipeline was already stopped
     */
    bool stop();

    /**
     * Adds Worker to the end of the Pipeline
     * @param worker Pointer to to be added Worker
     */
    void push_worker(Worker *worker);

    /**
     * Debug the current pipeline configuration
     * @return String-Vector with Provider/Worker ids in correct order
     */
    std::vector<std::string> get_pipeline();
};


#endif //DUNE_PIPELINE_PIPELINE_H
