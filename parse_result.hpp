#pragma once

#include <iostream>
#include <string>

#include "option.hpp"

using namespace std;

class ParseResult {
   private:
    Option *_opt;
    string _value;

   public:
    ParseResult(Option *opt, string value);

    string get_value();
    Option *get_opt();

    friend ostream &operator<<(ostream &out, const ParseResult &res);
};
