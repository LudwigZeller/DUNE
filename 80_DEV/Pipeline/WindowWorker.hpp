#ifndef DUNE_PIPELINE_WINDOWWORKER_HPP
#define DUNE_PIPELINE_WINDOWWORKER_HPP

#include "Worker.hpp"
#include <chrono>
#include <thread>
#include <utility>
#include <atomic>

class WindowWorker : public Worker{
private:
    Window *m_window;

public:
    WindowWorker(std::string id, Window *window) : Worker(std::move(id), MatIOType::VEC_3_CHAR_8, MatIOType::NONE){
        clog(info) << this->get_id() << " initialized!" << std::endl;
        m_window = window;
    };

    void start_up() override {};

    void work() override{
        m_window->render_matrix(std::move(m_work_matrix));
    };
};


#endif //DUNE_PIPELINE_WINDOWWORKER_HPP
