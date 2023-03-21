#pragma once

#include <string>

enum ServerMode {
    UNKNOWN,
    TCP,
    UDP
};

std::string mode_to_str(ServerMode mode);
