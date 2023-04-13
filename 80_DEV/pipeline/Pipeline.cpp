#include "Pipeline.hpp"
#include "MatIO.hpp"

Pipeline::Pipeline(Provider *provider) {
    m_provider = provider;
}

Pipeline::~Pipeline() {
    this->stop();
    this->m_provider->stop();
    //! Are declared as stack variables!!!
    /*delete m_provider;
    for (Worker *worker: m_pipeline) delete worker;*/
}

bool Pipeline::start() {
    if (m_running) return false;
    clog(info) << "Starting Pipeline!" << std::endl;
    {
        std::lock_guard<std::mutex> lock(m_pipeline_mutex);
        if(m_provider->start())
            //! Extra waiting here is required to supress error
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        for (Worker *worker: m_pipeline) worker->start();
        m_running = true;
    }

    //!! Wait for camera
    while(m_provider->pop().empty());

    m_thread = std::thread{[this] {
        clog(info) << "Pipeline running in " << std::this_thread::get_id() << std::endl;
        cv::Mat transfer;
        while (this->m_running) {
            std::lock_guard<std::mutex> lock(m_pipeline_mutex);
            transfer = std::move(m_provider->pop());
            for (Worker *worker: m_pipeline) {
                if (!transfer.empty()) worker->push(std::move(transfer));
                transfer = std::move(worker->pop());
            }
        }
    }};
    clog(info) << "Started Pipeline!" << std::endl;
    return true;
}

bool Pipeline::stop() {
    if (!m_running) return false;
    clog(info) << "Stopping Pipeline!" << std::endl;
    {
        std::lock_guard<std::mutex> lock(m_pipeline_mutex);
        //m_provider->stop();
        for (Worker *worker: m_pipeline) worker->stop();
        m_running = false;
    }
    m_thread.join();
    clog(info) << "Stopped Pipeline!" << std::endl;
    return true;
}

void Pipeline::push_worker(Worker *worker) {
    std::lock_guard<std::mutex> lock(m_pipeline_mutex);
    
    if(m_pipeline.size() <= 0 && m_provider->m_output_type.index != worker->m_input_type.index)
    {
        throw std::runtime_error(std::string("Mat types of \"") + m_provider->get_id() + "\" [" + m_provider->m_output_type.name + "] (prev) and \"" + worker->get_id() + "\" [" + worker->m_output_type.name + "] (new) are unequal!");
    }
    else if(m_pipeline.size() > 0 && m_pipeline.back()->m_output_type.index != worker->m_input_type.index)
    {
        throw std::runtime_error(std::string("Mat types of \"") + m_pipeline.back()->get_id() + "\" [" + m_pipeline.back()->m_output_type.name + "] (prev) and \"" + worker->get_id() + "\" [" + worker->m_output_type.name + "] (new) are unequal!");
    }

    m_pipeline.push_back(worker);
    if (m_running)
        worker->start();
}

std::vector<std::string> Pipeline::get_pipeline() {
    std::lock_guard<std::mutex> lock(m_pipeline_mutex);
    std::vector<std::string> ids{m_provider->get_id()};
    for (auto worker: m_pipeline) { ids.push_back(worker->get_id()); }
    return ids;
}