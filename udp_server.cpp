#include "udp_server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "server.hpp"

// UDP specific definitions

#define OPCODE_REQUEST 0
#define OPCODE_RESPONSE 1

#define STATUS_OK 0
#define STATUS_ERR 1

void UdpServer::begin(string host, int port) {
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

    // create socket endpoint
    _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socketfd <= 0) {
        cerr << "ERROR: Failed to open socket endpoint." << endl;
        exit(EXIT_FAILURE);
    }

    // connect
    // as-per man docs, connect should only set the default server address for UDP communication
    if (connect(_socketfd, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        cerr << "ERROR: Failed to connect socket." << endl;
        exit(EXIT_FAILURE);
    }
}

void UdpServer::send_payload(string payload) {
    int len = payload.length() + 1 + 2;  // + null termination + 2 bytes of header

    char buf[len];
    buf[0] = OPCODE_REQUEST;
    buf[1] = payload.length() + 1;  // + null termination

    const char *payload_raw = payload.c_str();
    // copy into buffer
    std::copy(&payload_raw[0], &payload_raw[payload.length()], &buf[2]);

    ssize_t sent = send(_socketfd, buf, len, 0);

    if (sent < 0) {
        cerr << "ERROR: Failed to send payload to server." << endl;
        end_gracefully();
        exit(EXIT_SUCCESS);
    }
}

// blocking OP
void UdpServer::await_response() {
    char buf[BUFSIZE];

    bzero(buf, BUFSIZE);
    ssize_t received = recv(_socketfd, buf, BUFSIZE, 0);

    if (received < 0) {
        cerr << "ERROR: Failed to receive payload from server." << endl;
        end_gracefully();
        exit(EXIT_FAILURE);
    }

    // missing format bytes
    if (received < 4) {
        cerr << "ERROR: Bad response format." << endl;
        end_gracefully();
        exit(EXIT_FAILURE);
    }

    // check opcode (first byte) is response
    // OPCODE_REQUEST
    // OPCODE_RESPONSE

    if (buf[0] != OPCODE_RESPONSE) {
        cerr << "ERROR: Expected a response from server. Wrong opcode." << endl;
        exit(EXIT_FAILURE);
    }

    // copy payload from the buffer

    int payload_length = buf[2];

    char payload[payload_length];
    std::copy(&buf[3], &buf[3 + payload_length], payload);

    // check status code and print based on that (ERR / OK)
    // STATUS_OK
    // STATUS_ERR
    if (buf[1] != STATUS_OK) {
        cout << "ERR:" << payload << endl;
        return;
    }

    cout << "OK:" << payload << endl;
}

void UdpServer::end_gracefully() {
    end();
}

void UdpServer::end() {
    close(_socketfd);
}
