#include <vector>

#include "option.hpp"
#include "parse_result.hpp"
#include "server_config.hpp"

/**
 * Create our own CLI argument parser, because why not.
 *
 * Supports only options (switches), no positional arguments.
 */
class ArgParser {
   private:
    vector<Option *> _options;

   public:
    ArgParser(vector<Option *> options);

    string get_help();
    vector<ParseResult> parse(int argc, const char *argv[]);
};
