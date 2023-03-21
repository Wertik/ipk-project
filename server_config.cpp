#include "server_config.hpp"

#include "server_mode.hpp"

void ServerConfig::ServerConfig::set_port(int port) {
    this->_port = port;
}

void ServerConfig::set_mode(ServerMode mode) {
    this->_mode = mode;
}

void ServerConfig::set_host(string host) {
    this->_host = host;
}

string ServerConfig::get_host() {
    return this->_host;
}

int ServerConfig::get_port() {
    return this->_port;
}

ServerMode ServerConfig::get_mode() {
    return this->_mode;
}

bool ServerConfig::is_complete() {
    // todo: enforce with a constructor somehow? idk
    return this->_mode != UNKNOWN && !this->_host.empty() && this->_port != -1;
}

ostream &operator<<(ostream &out, const ServerConfig &config) {
    out << mode_to_str(config._mode) << "://" << config._host << ":" << config._port;
    return out;
}
