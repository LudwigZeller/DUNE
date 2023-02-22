//
// Created by eragon on 11/16/22.
//

#ifndef DUNE_PIPELINE_WORKER_HPP
#define DUNE_PIPELINE_WORKER_HPP

#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <utility>
#include <atomic>
#include "../utils.hpp"
#include "MatIO.hpp"

class Worker {
private:
    struct SharedMatrix {
        cv::Mat matrix;
        std::mutex mutex;
        std::atomic_bool is_new = false;
    };

    SharedMatrix m_input;
    SharedMatrix m_output;
    std::atomic_bool m_running = false;
    std::thread m_thread;

protected:
    cv::Mat m_work_matrix;
    std::string m_id;

    /**
     * Concurrent worker function, abstracted to be easy to use
     */
    virtual void work() = 0;
    /**
     * Runs on the start of the Worker, runs on the calling thread
     */
    virtual void start_up() = 0;
public:
    const MatIOType::MatIOType_t m_input_type, m_output_type;

    /**
     * Constructs Worker with ID and sets Matrix IO types
     * @param id ID of the Worker
     * @param input Input matrix type
     * @param output Output matrix type
     */
    explicit Worker(std::string id, MatIOType::MatIOType_t input, MatIOType::MatIOType_t output) : m_id(std::move(id)), m_input_type(input), m_output_type(output) {
        clog(info) << "Constructed " << m_id << std::endl;
    };

    virtual ~Worker() {
        this->stop();
    };

    /**
     * Pushes a new matrix onto the Worker.
     * Warns if Worker is overflowing
     * @param matrix R-Value of new matrix
     * @return If it overwrote the Matrix in Queue
     */
    bool push(cv::Mat &&matrix) {
        std::lock_guard<std::mutex> lock(m_input.mutex);
        bool override = false;
        if (m_input.is_new) {
            clog(warn) << this->get_id() << ": Input Matrix provided while one is in queue! - Worker too slow?" << std::endl;
            override = true;
        }
        m_input.matrix = std::move(matrix);
        m_input.is_new = true;
        return override;
    }

    /**
     * Pops finished Matrix of the worker
     * @return R-Value of finished matrix
     */
    cv::Mat &&pop() {
        std::lock_guard<std::mutex> lock(m_output.mutex);
        m_output.is_new = false;
        return std::move(m_output.matrix);
    }

    /**
     * Starts the work thread of the worker
     * @return If the worker was already running
     */
    bool start() {
        if (m_running) return false;
        m_running = true;
        m_thread = std::thread{[this] {
            //clog(info) << "Started thread " << std::this_thread::get_id() << ", for " << this->m_id << std::endl;
            this->start_up();
            while (this->m_running) {
                if (!this->m_input.is_new) {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    continue;
                }
                {
                    std::lock_guard<std::mutex> lock(this->m_input.mutex);
                    this->m_input.is_new = false;
                    this->m_work_matrix = std::move(this->m_input.matrix);
                }
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

    /**
     * Joins the work thread of the worker
     * @return If the worker was already stopped
     */
    bool stop() {
        if (!m_running) return false;
        m_running = false;
        m_thread.join();
        clog(info) << "Stopped " << this->get_id() << "!" << std::endl;
        return true;
    }

    /**
     * Gets ID
     * @return Worker ID
     */
    [[nodiscard]] std::string get_id() const { return m_id; }
};


#endif //DUNE_PIPELINE_WORKER_HPP