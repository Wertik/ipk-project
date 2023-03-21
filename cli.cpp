#include "cli.hpp"

#include <vector>

#include "arg_parser.hpp"
#include "option.hpp"
#include "parse_result.hpp"
#include "server_config.hpp"

using namespace std;

ServerConfig process_cli(int argc, const char *argv[]) {
    Option host("host", "h", "Set the host of the calculator server.", true);
    Option port("port", "p", "Set the port of the calculator server.", true);
    Option mode("mode", "m", "Set the mode of communication. Supported modes: UDP, TCP.", true);
    Option help("help", "hwastaken", "Print this help page, exit.", false);

    vector<Option *> options({
        &help,
        &mode,
        &host,
        &port,
    });

    ArgParser parser(options);

    // process the options

    vector<ParseResult> args = parser.parse(argc, argv);

    ServerConfig config;

    for (ParseResult arg : args) {
        // print help, exit the program
        if (help == arg.get_opt()) {
            cout << parser.get_help();
            exit(EXIT_SUCCESS);
        }

        // validate and set the current mode
        if (mode == arg.get_opt()) {
            // valid options: udp, tcp (case insensitive)
            string val = arg.get_value();

            if (strtolower(val).compare("udp") == 0) {
                config.set_mode(ServerMode::UDP);
            } else if (strtolower(val).compare("tcp") == 0) {
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

    return config;
}

// tolower helper method
// cpp std/string has no to lower & to upper? hello?
// returns a new string
string strtolower(string str) {
    string out;
    for (auto it = str.begin(); it != str.end(); it++) {
        out.push_back(tolower(*it));
    }
    return out;
}