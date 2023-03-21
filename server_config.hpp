#pragma once

#include <iostream>
#include <string>

#include "server.hpp"
#include "server_mode.hpp"
#include "tcp_server.hpp"
#include "udp_server.hpp"

using namespace std;

class ServerConfig {
   private:
    int _port = -1;
    string _host = "localhost";
    ServerMode _mode = UNKNOWN;

   public:
    ServerConfig() {}

    int get_port();
    void set_port(int port);

    ServerMode get_mode();
    void set_mode(ServerMode mode);

    string get_host();
    void set_host(string host);

    bool is_complete();

    Server *create_server();

    friend ostream &operator<<(ostream &out, const ServerConfig &config);
};
