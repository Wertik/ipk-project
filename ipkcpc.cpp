#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "cli.hpp"
#include "server_config.hpp"
#include "tcp_server.hpp"
#include "udp_server.hpp"

using namespace std;

// handle sigint, set flag, react in read
volatile sig_atomic_t sig_exit = 0;
void handler(int sig) {
    sig_exit = 1;
}

int main(int argc, const char *argv[]) {
    // Load config from CLI arguments.
    ServerConfig config = process_cli(argc, argv);

    // Register sigint handler
    struct sigaction sigact = {0};

    // block SIGINT
    sigemptyset(&sigact.sa_mask);
    sigaddset(&sigact.sa_mask, SIGINT);
    // handle
    sigact.sa_flags = 0;
    sigact.sa_handler = handler;

    // register
    if (sigaction(SIGINT, &sigact, NULL) == -1) {
        cerr << "ERROR: Failed to register sigint handler." << endl;
        exit(EXIT_FAILURE);
    }

    // Connect to the server

    Server *server = config.create_server();
    server->begin(config.get_host(), config.get_port());

    // Read from stdin, send query, wait for response

    string in;
    while (getline(cin, in)) {
        sleep(2);
        if (sig_exit == 1) {
            // graceful enough?
            server->end_gracefully();
            break;
        }

        server->send_payload(in);

        // blocking
        server->await_response();
    }

    // End connection

    server->end();
    return 0;
}
