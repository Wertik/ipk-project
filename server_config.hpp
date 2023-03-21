#pragma once

#include <sstream>
#include <string>

#include "server_mode.hpp"

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

    friend ostream &operator<<(ostream &out, const ServerConfig &config);
};
