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

enum MatchResult {
    NONE,
    SHORT,
    LONG,
};

class Option {
   private:
    string _longhand;
    string _shorthand;
    string _description;
    bool _has_val;

   public:
    Option(string longhand, string shorthand, string description, bool has_val) {
        this->_longhand = longhand;
        this->_shorthand = shorthand;
        this->_description = description;
        this->_has_val = has_val;
    }

    string get_long() {
        return "--" + this->_longhand;
    }

    string get_short() {
        return "-" + this->_shorthand;
    }

    string get_description() {
        return this->_description;
    }

    bool has_val() {
        return this->_has_val;
    }

    // whether an argument matches this option
    // either shorthand or longhand
    // -- & - get prefixed
    MatchResult match(string arg) {
        // argument contains the longhand on the 0th position
        if (arg.rfind(this->get_long(), 0) == 0) {
            return MatchResult::LONG;
        }
        if (this->get_short().compare(arg) == 0) {
            return MatchResult::SHORT;
        }
        return MatchResult::NONE;
    }

    bool operator==(Option *b) {
        return b->get_long().compare(this->get_long()) == 0;
    }
};

class ParseResult {
   private:
    Option *_opt;
    string _value;

   public:
    ParseResult(Option *opt, string value) {
        this->_opt = opt;
        this->_value = value;
    }

    string get_value() {
        return this->_value;
    }

    Option *get_opt() {
        return this->_opt;
    }

    friend ostream &operator<<(ostream &out, const ParseResult &res) {
        out << res._opt->get_long() << "(" << (res._value.empty() ? "none" : res._value) << ")" << endl;
        return out;
    }
};

vector<ParseResult> parse_options(vector<Option *> options, int argc, const char *argv[]) {
    vector<ParseResult> args;

    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);

        // get the option
        Option *o = nullptr;
        string value;
        MatchResult res = NONE;

        for (Option *lopt : options) {
            // match the option
            res = lopt->match(arg);

            if (res != NONE) {
                o = lopt;

                // parse value

                if (o->has_val()) {
                    if (res == SHORT) {
                        // -m MODE
                        // take the next argument if possible
                        if (++i == argc) {
                            cerr << "ERROR: Option " << arg << " requires a value." << endl;
                            exit(EXIT_FAILURE);
                        }

                        value = string(argv[i]);
                    } else if (res == LONG) {
                        // --mode=MODE
                        // split with first =
                        size_t idx = arg.find_first_of('=');
                        if (idx == -1) {
                            cerr << "ERROR: Option " << arg << " requires a value." << endl;
                            exit(EXIT_FAILURE);
                        }
                        value = arg.substr(idx + 1);
                    }
                } else {
                    if (res == LONG && string(arg).find_first_of('=') != -1) {
                        cerr << "ERROR: Option " << arg << " doesn't require a value." << endl;
                        exit(EXIT_FAILURE);
                    }
                }

                break;
            }
        }

        if (res == NONE || o == nullptr) {
            cerr << "ERROR: Invalid option " << arg << endl;
            exit(EXIT_FAILURE);
        }

        ParseResult parseRes(o, value);

        args.push_back(parseRes);
    }
    return args;
}

string get_help(vector<Option *> options) {
    stringstream help(
        "Usage: ./main [OPTIONS]\n\nOptions:\n", ios_base::app | ios_base::out);

    // append options

    for (Option *o : options) {
        help << "\t" << o->get_long() << (o->has_val() ? "=VALUE" : "");
        help << "|" << o->get_short() << (o->has_val() ? " VALUE" : "");
        help << "\t" << o->get_description() << endl;
    }

    return help.str();
}

enum ServerMode {
    UNKNOWN,
    TCP,
    UDP
};

string mode_to_str(ServerMode mode) {
    switch (mode) {
        case TCP:
            return "tcp";
        case UDP:
            return "udp";
        default:
            return "unknown";
    }
}

class ServerConfig {
   private:
    int _port = -1;
    string _host;
    ServerMode _mode = UNKNOWN;

   public:
    ServerConfig() {}

    void set_port(int port) {
        this->_port = port;
    }

    void set_mode(ServerMode mode) {
        this->_mode = mode;
    }

    void set_host(string host) {
        this->_host = host;
    }

    string get_host() {
        return this->_host;
    }

    int get_port() {
        return this->_port;
    }

    ServerMode get_mode() {
        return this->_mode;
    }

    bool is_complete() {
        // todo: enforce with a constructor somehow? idk
        return this->_mode != UNKNOWN && !this->_host.empty() && this->_port != -1;
    }

    friend ostream &operator<<(ostream &out, const ServerConfig &config) {
        out << mode_to_str(config._mode) << "://" << config._host << ":" << config._port;
        return out;
    }
};

// cpp std/string has no to lower & to upper? hello?
// returns a new string
string strlower(string str) {
    string out;
    for (auto it = str.begin(); it != str.end(); it++) {
        out.push_back(tolower(*it));
    }
    return out;
}

int main(int argc, const char *argv[]) {
    ServerConfig config;

    Option host("host", "h", "Sets the host of the calc server.", true);
    Option port("port", "p", "Sets the port of the calc server.", true);
    Option mode("mode", "m", "Sets the mode of communication. Supported modes: UDP, TCP.", true);
    Option help("help", "hwastaken", "Prints the help information.", false);

    vector<Option *> options({
        &help,
        &mode,
        &host,
        &port,
    });

    // process the options

    vector<ParseResult> args = parse_options(options, argc, argv);

    for (ParseResult arg : args) {
        // print help, exit the program
        if (help == arg.get_opt()) {
            cout << get_help(options);
            exit(EXIT_SUCCESS);
        }

        // validate and set the current mode
        if (mode == arg.get_opt()) {
            // valid options: udp, tcp (case insensitive)
            string val = arg.get_value();

            if (strlower(val).compare("udp") == 0) {
                config.set_mode(ServerMode::UDP);
            } else if (strlower(val).compare("tcp") == 0) {
                config.set_mode(ServerMode::TCP);
            } else {
                cerr << "ERROR: Invalid mode " << val << "." << endl;
                exit(EXIT_FAILURE);
            }
        }

        // validate and set the current port
        if (port == arg.get_opt()) {
            // todo: handle stoi error
            try {
                int port_parsed = stoi(arg.get_value());
                config.set_port(port_parsed);
            } catch (invalid_argument const &ex) {
                cerr << "ERROR: Port has to be a number. " << arg.get_value() << " is not a number." << endl;
                exit(EXIT_FAILURE);
            }
        }

        // nothing to validate?
        if (host == arg.get_opt()) {
            config.set_host(arg.get_value());
        }
    }

    if (!config.is_complete()) {
        cerr << "ERROR: Server configuration incomplete." << endl;
        exit(EXIT_FAILURE);
    }

    cerr << "[VERBOSE]: Running with config: " << config << endl;

    int sfd = start_socket("localhost", 2065);

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
