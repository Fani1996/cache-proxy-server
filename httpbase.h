#ifndef _HTTPBASE__H
#define _HTTPBASE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "socket.h"


class httpBase {
protected:
    std::string content;
    std::string payload;

    std::vector<std::string> meta;
    std::unordered_map<std::string, std::string> headerpair;

    void meta_parser(std::string meta);
    void header_parser();
    void payload_parser();

    void parser();
};

#endif
