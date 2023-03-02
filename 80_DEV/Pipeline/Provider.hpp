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

/**
 * The starting piece of the pipeline.
 * Produces matrices for the pipeline to work on
 */
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

    /**
     * Concurrent producer function, abstracted to be easy to use
     */
    virtual void work() = 0;

public:
    const MatIOType::MatIOType_t m_output_type;

    /**
     * Constructs Provider with ID and sets production matrix type
     * @param id Id of the Producer
     * @param output Output matrix type
     */
    explicit Provider(std::string id, MatIOType::MatIOType_t output) : m_id(std::move(id)), m_output_type(output) {
        clog(info) << "Constructed " << m_id << std::endl;
    };

    virtual ~Provider() {
        this->stop();
    };

    /**
     * Pops the produced matrix from the provider
     * @return R-Value finished matrix
     */
    cv::Mat &&pop() {
        std::lock_guard<std::mutex> lock(m_output.mutex);
        m_output.is_new = false;
        return std::move(m_output.matrix);
    }

    /**
     * Starts the work thread of the provider
     * @return If the worker was already running
     */
    bool start() {
        if (m_running) return false;
        m_running = true;
        m_thread = std::thread{[this] {
            while (this->m_running) {
                this->work();
                //! This will now be ignored because of launch stability
                //if (this->m_output.is_new)
                    //clog(warn) << this->get_id() << ": Output Matrix was not picked up! - Error in Pipeline?"
                    //           << std::endl;
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
     * Joins the work thead of the provider
     * @return If the provider was already stopped
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
     * @return Provider ID
     */
    [[nodiscard]] std::string get_id() const { return m_id; }
};


#endif //DUNE_PIPELINE_PROVIDER_HPP
