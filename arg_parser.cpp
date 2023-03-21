#include "arg_parser.hpp"

#include <sstream>
#include <vector>

#include "match_result.hpp"
#include "option.hpp"
#include "parse_result.hpp"
#include "server_config.hpp"

ArgParser::ArgParser(vector<Option *> options) {
    this->_options = options;
};

vector<ParseResult> ArgParser::parse(int argc, const char *argv[]) {
    vector<ParseResult> args;

    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);

        // get the option
        Option *o = nullptr;
        string value;
        MatchResult res = NONE;

        for (Option *lopt : _options) {
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

string ArgParser::get_help() {
    stringstream help(
        "Usage: ./main [OPTIONS]\n\nOptions:\n", ios_base::app | ios_base::out);

    // append options

    for (Option *o : _options) {
        help << "\t" << o->get_long() << (o->has_val() ? "=VALUE" : "");
        help << "|" << o->get_short() << (o->has_val() ? " VALUE" : "");
        help << "\t" << o->get_description() << endl;
    }

    return help.str();
}
