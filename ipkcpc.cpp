#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "arg_parser.hpp"
#include "cli.hpp"
#include "option.hpp"
#include "server_config.hpp"

#define BUFSIZE 1024

using namespace std;

int start_socket(const char *host, int port) {
    int socketfd;
    int bytesrx;
    int bytestx;
    socklen_t serverlen;

    struct hostent *server;
    struct sockaddr_in server_address;

    char buf[BUFSIZE];

    // dns resolution
    if ((server = gethostbyname(host)) == NULL) {
        fprintf(stderr, "ERROR: no such host as %s\n", host);
        exit(EXIT_FAILURE);
    }

    cout << "port:" << port << endl;

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    /* tiskne informace o vzdalenem soketu */
    printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    if (connect(socketfd, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    return socketfd;
}

void disconnect(int socketfd) {
    close(socketfd);
}

void send_message(int socketfd, string content) {
    ssize_t sent = send(socketfd, content.c_str(), content.length() + 1, 0);

    if (sent < 0) {
        // todo
        perror("ERROR in sendto");
    }

    cout << "Sent: " << content << endl;
}

string await_response(int socketfd) {
    char buf[BUFSIZE] = {0};
    ssize_t received = recv(socketfd, buf, BUFSIZE, 0);

    if (received < 0) {
        // todo
        perror("ERROR in recvfrom");
    }

    string str(buf);
    return str;
}

int main(int argc, const char *argv[]) {
    // Load config from CLI arguments.
    ServerConfig config = process_cli(argc, argv);

    // Do networking stuff.

    int sfd = start_socket(config.get_host().c_str(), config.get_port());

    string in;
    // todo: handle false/err?
    while (getline(cin, in)) {
        send_message(sfd, in);

        string res = await_response(sfd);
        cout << "ECHO: " << res << endl;
    }

    disconnect(sfd);
    return 0;
}
