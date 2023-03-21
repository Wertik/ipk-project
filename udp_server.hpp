#pragma once

#include "server.hpp"

class UdpServer : public Server {
   private:
    int _socketfd;

   public:
    void begin(string host, int port);

    void send_payload(string payload);
    void await_response();

    void end_gracefully();
    void end();
};
