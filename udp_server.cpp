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

#define BUFSIZE 1024

#define OPCODE_REQUEST 0
#define OPCODE_RESPONSE 1

#define STATUS_OK 0
#define STATUS_ERR 1

void UdpServer::begin(string host, int port) {
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

    /* tiskne informace o vzdalenem soketu */
    // todo
    printf("[VERBOSE]: Server UDP socket: %s:%d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socketfd <= 0) {
        // todo
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    if (connect(_socketfd, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        // todo
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }
}

void UdpServer::send_payload(string payload) {
    ssize_t sent = send(_socketfd, payload.c_str(), payload.length() + 1, 0);

    if (sent < 0) {
        // todo
        perror("ERROR in sendto");
        end_gracefully();
        exit(EXIT_SUCCESS);
    }
}

// blocking OP
void UdpServer::await_response() {
    char buf[BUFSIZE];

    bzero(buf, BUFSIZE);
    ssize_t received = recv(_socketfd, buf, BUFSIZE, MSG_DONTWAIT);

    if (received < 0) {
        // todo
        perror("ERROR in recvfrom");
        end_gracefully();
        exit(EXIT_SUCCESS);
    }

    // check opcode (first byte) is response
    // OPCODE_REQUEST
    // OPCODE_RESPONSE

    if (buf[0] != OPCODE_RESPONSE) {
        cerr << "ERROR: Expected a response from server. Wrong opcode." << endl;
        exit(EXIT_FAILURE);
    }

    // copy paylload from the buffer

    int payload_length = buf[2];

    char payload[payload_length];
    std::copy(&buf[3], &buf[BUFSIZE - 1], payload);

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
