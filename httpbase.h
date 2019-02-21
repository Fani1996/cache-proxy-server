#ifndef _HTTPBASE__H
#define _HTTPBASE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"


class httpBase {
protected:
    std::string content;
    std::string payload;

    std::vector<std::string> meta;
    std::unordered_map<std::string, std::string> headerpair;

    std::vector<std::string> split(const std::string &s, char delim);

    httpBase(std::string ct):content(ct){};
    void meta_parser(std::string meta);
    void header_parser(std::string line);

    void recv_http_1_0(HttpSocket & sk);
    void recv_http_1_1(HttpSocket & sk, int type);

    int recv_header(HttpSocket& sk);
    void recv_chunk(HttpSocket& sk);
    void recv_length(HttpSocket& sk);

    void send_400_bad_request(HttpSocket& sk);
    void send(HttpSocket sk);
};

#endif

