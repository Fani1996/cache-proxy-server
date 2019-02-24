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
    std::string header;
    std::vector<std::string> meta;
    std::unordered_map<std::string, std::string> headerpair;
    std::unordered_map<std::string, std::string> cache_control;

    std::vector<std::string> split(const std::string &s, char delim);

    void meta_parser(std::string meta);
    void header_parser(std::string line);
    void cache_control_parser();

    void recv_http_1_0(HttpSocket & sk);
    void recv_http_1_1(HttpSocket & sk, int type);

    int recv_header(HttpSocket& sk);
    void recv_chunk(HttpSocket& sk);
    void recv_length(HttpSocket& sk);


    void send_400_bad_request(HttpSocket& sk);
    void send_502_bad_gateway(HttpSocket& sk);
    
public:
    //constructor.
    httpBase() {}
    httpBase(std::string ct) : content(ct) {}

    std::string get_content();
    std::string get_header_kv(std::string key);
    void set_header_kv(std::string key, std::string value);

    void update_header(std::string header);
    void refresh();
    
    std::string get_cache_control(std::string key);

    void send(HttpSocket sk);
    bool can_cache();
};

#endif

