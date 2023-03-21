#include "option.hpp"

Option::Option(string longhand, string shorthand, string description, bool has_val) {
    this->_longhand = longhand;
    this->_shorthand = shorthand;
    this->_description = description;
    this->_has_val = has_val;
}

string Option::get_long() {
    return "--" + this->_longhand;
}

string Option::get_short() {
    return "-" + this->_shorthand;
}

string Option::get_description() {
    return this->_description;
}

bool Option::has_val() {
    return this->_has_val;
}

// whether an argument matches this option
// either shorthand or longhand
// -- & - get prefixed
MatchResult Option::match(string arg) {
    // argument contains the longhand on the 0th position
    if (arg.rfind(this->get_long(), 0) == 0) {
        return MatchResult::LONG;
    }
    if (this->get_short().compare(arg) == 0) {
        return MatchResult::SHORT;
    }
    return MatchResult::NONE;
}

bool Option::operator==(Option *b) {
    return b->get_long().compare(this->get_long()) == 0;
}
