#ifndef _HTTPBASE__H
#define _HTTPBASE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "socket.h"


class httpBase {
protected:
    std::vector<char> payload;

    std::vector<std::string> meta;
    std::unordered_map<std::string, std::string> headerpair;

    void meta_parser(std::string line);
    void header_parser(std::string header);

    void payload_parser(std::vector<char> content);
};

#endif
