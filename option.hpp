#pragma once
#include <string>

#include "match_result.hpp"

using namespace std;

class Option {
   private:
    string _longhand;
    string _shorthand;
    string _description;
    bool _has_val;

   public:
    Option(string longhand, string shorthand, string description, bool has_val);

    string get_long();

    string get_short();

    string get_description();

    bool has_val();

    // whether an argument matches this option
    // either shorthand or longhand
    // -- & - get prefixed
    MatchResult match(string arg);

    bool operator==(Option *b);
};