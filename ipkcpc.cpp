#include <iostream>
#include <string>

#include "cli.hpp"
#include "server_config.hpp"
#include "tcp_server.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    // Load config from CLI arguments.
    ServerConfig config = process_cli(argc, argv);

    // Connect to the server

    // todo: mode

    TcpServer server;
    server.begin(config.get_host(), config.get_port());

    // Send query, wait for response

    string in;
    while (getline(cin, in)) {
        server.send_payload(in);

        server.await_response();
    }

    // End connection

    server.end();
    return 0;
}
