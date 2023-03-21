#pragma once

#include <string>

using namespace std;

class Server {
   public:
    virtual void begin(string host, int port) = 0;
    virtual void send_payload(string payload) = 0;
    virtual string await_response() = 0;
    virtual void end() = 0;
};
