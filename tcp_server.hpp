#pragma once

#include "server.hpp"

class TcpServer : public Server {
   private:
    int _socketfd;

    void force_bye();

   public:
    void begin(string host, int port);
    void send_payload(string payload);
    string await_response();
    void end();
};