#include "tcp_server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "server.hpp"

void TcpServer::begin(string host, int port) {
    // Code taken from https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c
    // And slightly modified.

    // dns resolution
    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL) {
        cerr << "ERROR: No such host " << host << endl;
        exit(EXIT_FAILURE);
    }

    // build sockaddr info
    struct sockaddr_in server_address;

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    /* Vytvoreni soketu */
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketfd <= 0) {
        cerr << "ERROR: Failed to open socket endpoint." << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(_socketfd, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        cerr << "ERROR: Failed to connect socket." << endl;
        exit(EXIT_FAILURE);
    }
}

void TcpServer::send_payload(string payload) {
    if (payload.compare("BYE") == 0) {
        this->_end = true;
    }

    ssize_t sent = send(_socketfd, payload.c_str(), payload.length() + 1, 0);

    if (sent < 0) {
        cerr << "ERROR: Failed to send payload to server." << endl;
        end_gracefully();
        exit(EXIT_SUCCESS);
    }
}

// blocking OP
void TcpServer::await_response() {
    char buf[BUFSIZE];

    bzero(buf, BUFSIZE);
    ssize_t received = recv(_socketfd, buf, BUFSIZE, 0);

    if (received < 0) {
        cerr << "ERROR: Failed to receive payload from server." << endl;
        end_gracefully();
        exit(EXIT_SUCCESS);
    }

    string str(buf);

    if (str.compare("HELLO") == 0) {
        cout << str << endl;
    } else if (str.compare("BYE") == 0) {
        cout << str << endl;
        if (this->_end) {
            // we ended this connection, this is a confirmation
            end();
        } else {
            // we received from server BYE first
            end_gracefully();
        }
        exit(EXIT_SUCCESS);
    } else {
        cout << "RESULT " << str << endl;
    }
}

void TcpServer::end_gracefully() {
    send_payload("BYE");
    // don't wait for response
    end();
}

void TcpServer::end() {
    close(_socketfd);
}
