#pragma once

#include "Worker.hpp"

const static cv::Size ksize{9,9};

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class ColorizeWorker : public Worker
 * @brief Temporary matrix colorization worker
*/
class InterpolatorWorker : public Worker
{
private:
    std::atomic_bool m_threads_running = true;
    std::condition_variable m_cv_child, m_cv_parent;
    std::mutex m_mutex_child1, m_mutex_child2, m_mutex_parent;
    cv::Mat m_mat_reference1, m_mat_reference2;

    /*!! Top Left to Bottom Middle rectangle
      [####....]
      [####....]
      [####....]
    */
    cv::Rect m_mat_half1{
            CUTOFF_LEFT, CUTOFF_TOP,
            (CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2,
            CUTOFF_BOT - CUTOFF_TOP + 1
        };
    /*!! Top Left to Bottom Middle rectangle
      [....####]
      [....####]
      [....####]
    */
    cv::Rect m_mat_half2{
            (CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2 + CUTOFF_LEFT, CUTOFF_TOP,
            (CUTOFF_RIGHT - CUTOFF_LEFT + 1) / 2,
            CUTOFF_BOT - CUTOFF_TOP + 1
        };

    std::thread m_thread1{[&]{
        std::unique_lock ul1{m_mutex_child1};
        m_cv_child.wait(ul1);
        while(m_threads_running)
        {
            cv::medianBlur(m_mat_reference1, m_mat_reference1, 5);
            std::this_thread::yield();
            m_cv_parent.notify_all();
            m_cv_child.wait(ul1);
        }
    }};
    std::thread m_thread2{[&]{
        std::unique_lock ul2{m_mutex_child2};
        m_cv_child.wait(ul2);
        while(m_threads_running)
        {
            cv::medianBlur(m_mat_reference2, m_mat_reference2, 5);
            std::this_thread::yield();
            m_cv_parent.notify_all();
            m_cv_child.wait(ul2);
        }
    }};

public:
    InterpolatorWorker(std::string id): Worker{std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::VEC_3_CHAR_8}
    { /* No extra construction required */ }

    ~InterpolatorWorker()
    {
        m_threads_running = false;
        m_cv_child.notify_all();
        m_cv_parent.notify_all();
        m_thread1.join();
        m_thread2.join();
    }

protected:
    void start_up() override
    { /* Start up not required for now */ }

    void work() override
    {
        m_mat_reference1 = this->m_work_matrix(m_mat_half1);
        m_mat_reference2 = this->m_work_matrix(m_mat_half2);
        m_cv_child.notify_all();
        std::unique_lock ul3{m_mutex_parent};
        m_cv_parent.wait(ul3);
    }
};

};
