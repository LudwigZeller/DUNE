//
// Created by eragon on 19.10.22.
//
#include "Webserver.hpp"


Webserver::~Webserver() {
    this->m_running = false;
    int res = shutdown(m_socket_fd, SHUT_RDWR);
    if (res == -1)
        std::cerr << "Unable to close Socket: " << errno << std::endl;
    
    this->m_thread->join();
    delete this->m_thread;
}

void Webserver::create() {
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd == -1)
        throw WebserverException("Unable to open socket", errno);
    int opt = 1;
    // For Address already in use errors
    int res = setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (res == -1)
        throw WebserverException("Unable to set Operation Directive", errno);
}

void Webserver::listen(uint16_t port) {
    struct sockaddr_in address{
            AF_INET,
            htons(port),
            INADDR_ANY
    };

    int res = bind(m_socket_fd, (struct sockaddr *) &address, sizeof(address));
    if (res == -1)
        throw WebserverException("Unable to bind to Port", errno);

    res = ::listen(m_socket_fd, 3);
    if (res == -1)
        throw WebserverException("Unable to listen on Port", errno);

    this->m_running = true;

    auto serve = [address, this]() {
        while (this->m_running) {
            // ------------------ AWAIT REQUEST ------------------
            int addrlen = sizeof(address);
            int open_socket = accept(m_socket_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
            char buffer[1024] = {0};
            read(open_socket, buffer, 1024);
            std::string request(buffer);
            std::cout << "------------ INCOMING REQUEST ------------\n"
                      << request << std::endl;

            //  ------------------ Handling Request ------------------
            std::string response;
            if (request.find("POST") != std::string::npos)
                response = res_post(request);
            else if (request.find("GET") != std::string::npos)
                response = res_get();
            else
                response = res_error();


            // ------------------ RESPONSE ------------------
            send(open_socket, response.c_str(), response.length(), 0);
            std::cout << "---------------- RESPONSE ----------------\n"
                      << response << "\n"
                      << "------------------------------------------\n" << std::endl;

            close(open_socket);
        }
    };

    m_thread = new std::thread(serve);
}

std::string Webserver::res_get() {
    std::string res =
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/html; charset=utf-8\n"
            "Accept-Ranges: bytes\n"
            "Connection: close\n"
            "\n";

    std::ifstream index_file("index.html");
    if (index_file.is_open()) {
        while (index_file) {
            std::string line;
            std::getline(index_file, line);
            res += line;
        }
    } else {
        res +=
                "<html>"
                "<head><title>Test</title></head>"
                "<body><h1 text-align=\"center\">No index_file.html found!</h1></body>"
                "</html>";
    }
    return res;
}

std::string Webserver::res_post(const std::string &request) {
    std::string res =
            "HTTP/1.1 200 OK\n"
            "Content-Type: application/json; charset=US-ASCII\n"
            "Accept-Ranges: bytes\n"
            "Connection: close\n"
            "\n"
            "{";

    if (size_t pos = request.find("FILTER") != std::string::npos) {

        std::string filter = request.substr(request.find("FILTER:", pos) + 8 /* Len "FILTER: " */, request.find('\n', pos));
        switch (Data::DictionaryFilter.count(filter)) {
            case 1: {
                res += R"("Filter":")" + filter + "\"";
                Data::AtmoicData tmp = m_data.load();
                tmp.filter = Data::DictionaryFilter.at(filter);
                m_data.store(tmp);
            }
                break;
            case 0:
                res += R"("FilterUpdateError":"Filter not known")";
                break;
        }


    } else if ((pos = request.find("SIMULATION")) != std::string::npos) {

        std::string simulation = request.substr(request.find(':', pos) + 1, request.find('\n', pos));
        std::cout << simulation << std::endl;

    } else {
        return "HTTP/1.1 400 Bad Request\n"
               "Content-Type: application/json; charset=US-ASCII\n"
               "Accept-Ranges: bytes\n"
               "Connection: close\n"
               "\n"
               "{\"error\":\"No Filter / Simulation with this name\"}";
    }
    return res += "}";
}

std::string Webserver::res_error() {
    std::string res =
            "HTTP/1.1 405 Method Not Allowed\n"
            "Content-Type: application/json; charset=US-ASCII\n"
            "Accept-Ranges: bytes\n"
            "Connection: close\n"
            "\n"
            "{ \"ERROR\": \"Unsupported Protocol\" }";
    return res;
}

