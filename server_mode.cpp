#include "server_mode.hpp"

std::string mode_to_str(ServerMode mode) {
    switch (mode) {
        case TCP:
            return "tcp";
        case UDP:
            return "udp";
        default:
            return "unknown";
    }
}