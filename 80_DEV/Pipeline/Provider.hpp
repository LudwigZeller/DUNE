//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_PROVIDER_HPP
#define DUNE_PIPELINE_PROVIDER_HPP

#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <utility>
#include "../utils.hpp"

class Provider {
private:
    struct SharedMatrix {
        cv::Mat matrix;
        std::mutex mutex;
        std::atomic_bool is_new = false;
    };

    SharedMatrix m_output;
    std::atomic_bool m_running = false;
    std::thread m_thread;

protected:
    cv::Mat m_work_matrix;
    std::string m_id;

    virtual void work() = 0;

public:
    explicit Provider(std::string id) : m_id(std::move(id)) {
        clog(info) << "Constructed " << m_id << std::endl;
    };

    virtual ~Provider() {
        this->stop();
    };

    cv::Mat &&pop() {
        std::lock_guard<std::mutex> lock(m_output.mutex);
        m_output.is_new = false;
        return std::move(m_output.matrix);
    }

    bool start() {
        if (m_running) return false;
        m_running = true;
        m_thread = std::thread{[this] {
            while (this->m_running) {
                this->work();
                if (this->m_output.is_new)
                    clog(warn) << this->get_id() << ": Output Matrix was not picked up! - Error in Pipeline?"
                               << std::endl;
                {
                    std::lock_guard<std::mutex> lock(this->m_output.mutex);
                    this->m_output.matrix = std::move(this->m_work_matrix);
                    this->m_output.is_new = true;
                }
            }
        }};
        clog(info) << "Started " << this->get_id() << "!" << std::endl;
        return true;
    }

    bool stop() {
        if (!m_running) return false;
        m_running = false;
        m_thread.join();
        clog(info) << "Stopped " << this->get_id() << "!" << std::endl;
        return true;
    }

    [[nodiscard]] std::string get_id() const { return m_id; }
};


#endif //DUNE_PIPELINE_PROVIDER_HPP
