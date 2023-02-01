//
// Created by eragon on 19.10.22.
//
#ifndef WEBSERVER_WEBSERVER_HPP
#define WEBSERVER_WEBSERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>
#include <string>
#include <utility>
#include <thread>
#include <csignal>
#include <atomic>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>

namespace Data {
    enum class Filter {
        NORMAL, BLOCKCRAFT, DIFFERENCE, STRIPE
    };
    enum class Simulation {
        NONE, GAME
    };
    struct AtmoicData {
        Data::Filter filter;
        Data::Simulation simulation;
    };
    inline const std::map<std::string, Filter> DictionaryFilter = {
            {"NORMAL",     Filter::NORMAL},
            {"BLOCKCRAFT", Filter::BLOCKCRAFT},
            {"DIFFERENCE", Filter::DIFFERENCE},
            {"STRIPE",     Filter::STRIPE}
    };
    inline const std::map<std::string, Simulation> DictionarySimulation = {
            {"NONE", Simulation::NONE},
            {"GAME", Simulation::GAME}
    };
};

class Webserver {
private:
    int m_socket_fd = 0;
    std::thread *m_thread = nullptr;
    std::atomic<Data::AtmoicData> m_data;
    bool m_running = false;

    // --------- Responses ---------
    static std::string res_get();

    std::string res_post(const std::string &request);

    static std::string res_error();

public:
    ~Webserver();

    void create();

    void listen(uint16_t port);

    [[nodiscard]] Data::AtmoicData get_data() { return m_data.load(); }

    [[nodiscard]] int get_socket_fd() const { return m_socket_fd; }

    class WebserverException : public std::exception {
    private:
        std::string m_error;
        int m_error_code;
    public:
        explicit WebserverException(std::string error, int error_code) {
            m_error = std::move(error);
            m_error_code = error_code;
        }

        [[nodiscard]] const char *what() const noexcept override { return m_error.c_str(); }

        [[nodiscard]] int error_code() const noexcept { return m_error_code; }
    };
};


#endif //WEBSERVER_WEBSERVER_HPP
