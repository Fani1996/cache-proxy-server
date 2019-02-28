
#ifndef _HTTPBASE__H
#define _HTTPBASE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"


class httpBase {
protected:
    std::vector<char> content;
    std::vector<char> header;
    std::vector<char> payload;

    int header_len;
    int payload_len;
    
    std::vector<std::string> meta;
    //    std::unordered_map<std::string, std::string> headerpair;
    std::unordered_map<std::string, std::string> cache_control;

    std::vector<std::string> split(const std::string &s, char delim);

    void meta_parser();
    void header_parser(std::string line);
    void cache_control_parser();

    void recv_http_1_0(HttpSocket & sk);
    void recv_http_1_1(HttpSocket & sk, int type);

    int recv_header(HttpSocket& sk);
    void recv_chunk(HttpSocket& sk);
    void recv_length(HttpSocket& sk);
    
public:
    //constructor.
    httpBase() {}
    std::vector<char> get_content();
    std::string get_header_kv(std::string key);
    void set_header_kv(std::vector<char> key, std::vector<char> value);

    //    void update_header(std::string header);
    //std::vector<char> generate_header();
    //    void refresh();
    
    std::string get_cache_control(std::string key);

    void send(HttpSocket sk);
    bool can_store();
    bool no_cache();
    bool must_revalidate();
    void send_400_bad_request(HttpSocket& sk);
    void send_502_bad_gateway(HttpSocket& sk);
};

#endif

