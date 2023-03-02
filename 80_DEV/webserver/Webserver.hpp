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
    /**
     * All supported Filters
     */
    enum class Filter {
        NORMAL, BLOCKCRAFT, DIFFERENCE, STRIPE, PERLIN, PERLIN_REG, PERLIN_SHOW
    };
    /**
     * All supported Simulations
     */
    enum class Simulation {
        NONE, GAME
    };
    /**
     * Struct for storing currently active Filter and Simulation
     */
    struct AtmoicData {
        Data::Filter filter;
        Data::Simulation simulation;
    };
    /**
     * Look-Up-Table for Filters
     */
    inline const std::map<std::string, Filter> DictionaryFilter = {
            {"NORMAL",     Filter::NORMAL},
            {"BLOCKCRAFT", Filter::BLOCKCRAFT},
            {"DIFFERENCE", Filter::DIFFERENCE},
            {"STRIPE",     Filter::STRIPE},
            {"PERLIN",     Filter::PERLIN},
            {"PERLIN_REG", Filter::PERLIN_REG},
            {"PERLIN_SHOW",Filter::PERLIN_SHOW},
    };
    /**
     * Look-Up-Table for Simulations
     */
    inline const std::map<std::string, Simulation> DictionarySimulation = {
            {"NONE", Simulation::NONE},
            {"GAME", Simulation::GAME}
    };
};

/**
 * Class for managing a non-blocking and lightweight Webserver for managing API Request und updating data
 */
class Webserver {
private:
    int m_socket_fd = 0;
    std::thread *m_thread = nullptr;
    std::atomic<Data::AtmoicData> m_data;
    bool m_running = false;

    // --------- Responses ---------
    /**
     * Generates a HTML Get response for browsers
     * @return HTTP encoded Get String
     */
    static std::string res_get();

    /**
     * Generates a JSON Post response, with current webserver data
     * @param request Request String
     * @return HTTP encoded Post String
     */
    std::string res_post(const std::string &request);

    /**
     * Generates a JSON error response
     * @return HTTP encoded Error String
     */
    static std::string res_error();

public:
    ~Webserver();

    /**
     * Opens a socket and sets the directive
     * @throws WebserverException
     */
    void create();

    /**
     * Starts the webserver on the specified port and spawn a thread which accepts incoming requests
     * @param port Listening Port
     * @throws WebserverException
     */
    void listen(uint16_t port);

    /**
     * Gets the webserver-data concurrently
     * @return The current state of the webserver
     */
    [[nodiscard]] Data::AtmoicData get_data() { return m_data.load(); }

    /**
     * Gets the Socket File-Descriptor
     * @return File-Descriptor
     */
    [[nodiscard]] int get_socket_fd() const { return m_socket_fd; }

    class WebserverException : public std::exception {
    private:
        std::string m_error;
        int m_error_code;
    public:
        /**
         * Constructs the WebserverException
         * @param error What caused the error
         * @param error_code The "errno" Error Code
         */
        explicit WebserverException(std::string error, int error_code) {
            m_error = std::move(error);
            m_error_code = error_code;
        }

        /**
         * Gets the error description
         * @return C-String Errormessage
         */
        [[nodiscard]] const char *what() const noexcept override { return m_error.c_str(); }

        /**
         * Gets the error code
         * @return "errno" Error Code
         */
        [[nodiscard]] int error_code() const noexcept { return m_error_code; }
    };
};


#endif //WEBSERVER_WEBSERVER_HPP
