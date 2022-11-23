#ifndef DUNE_PIPELINE_WINDOWWORKER_HPP
#define DUNE_PIPELINE_WINDOWWORKER_HPP

#include "Worker.hpp"
#include "../Window.hpp"
#include <chrono>
#include <thread>
#include <utility>
#include <atomic>

class WindowWorker : public Worker{
private:
    Window *m_window;

public:
    WindowWorker(std::string id, Window *window) : Worker(std::move(id)){
        clog(info) << this->get_id() << " initialized!" << std::endl;
        m_window = window;
    };

    void start_up() override {
        while(!m_window->initialized());
        glfwMakeContextCurrent(m_window->getwndptr());
    }

    void work() override{
        //glfwMakeContextCurrent(m_window->getwndptr());
        draw_frame(cv::Size((int) m_window->width(), (int) m_window->height()), m_work_matrix);
        clog(info) << this->get_id() << " finished work!" << std::endl;
    };
};


#endif //DUNE_PIPELINE_WINDOWWORKER_HPP
