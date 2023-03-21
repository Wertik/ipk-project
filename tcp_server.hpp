#pragma once

#include "server.hpp"

class TcpServer : public Server {
   private:
    int _socketfd;
    bool _end = false;

   public:
    void begin(string host, int port);
    void send_payload(string payload);
    void await_response();
    void end_gracefully();
    void end();
};
