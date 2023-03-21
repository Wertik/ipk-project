#include "parse_result.hpp"

ParseResult::ParseResult(Option *opt, string value) {
    this->_opt = opt;
    this->_value = value;
}

string ParseResult::get_value() {
    return this->_value;
}

Option *ParseResult::get_opt() {
    return this->_opt;
}

ostream &operator<<(ostream &out, const ParseResult &res) {
    out << res._opt->get_long() << "(" << (res._value.empty() ? "none" : res._value) << ")" << endl;
    return out;
}