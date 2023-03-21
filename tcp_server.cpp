#include "tcp_server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define BUFSIZE 1024

void TcpServer::begin(string host, int port) {
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
    printf("[VERBOSE]: Server TCP socket: %s:%d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
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

void TcpServer::send_payload(string payload) {
    ssize_t sent = send(_socketfd, payload.c_str(), payload.length() + 1, 0);

    if (sent < 0) {
        // todo
        perror("ERROR in sendto");
        force_bye();
        exit(EXIT_SUCCESS);
    }
}

string TcpServer::await_response() {
    char buf[BUFSIZE];

    bzero(buf, BUFSIZE);
    ssize_t received = recv(_socketfd, buf, BUFSIZE, 0);

    if (received < 0) {
        // todo
        perror("ERROR in recvfrom");
        force_bye();
        exit(EXIT_SUCCESS);
    }

    string str(buf);

    if (str.compare("HELLO") == 0) {
        cout << str << endl;
    } else if (str.compare("BYE") == 0) {
        cout << str << endl;
    } else {
        cout << "RESULT " << str << endl;
    }
    return str;
}

void TcpServer::force_bye() {
    send_payload("BYE");
    // don't wait for response
    end();
}

void TcpServer::end() {
    close(_socketfd);
}
