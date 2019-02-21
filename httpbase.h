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

    void meta_parser(std::string meta);
    void header_parser(std::string line);
    void recv_header(HttpSocket &sk);
    void recv_http1-0(HttpSocket & sk);

    void recv_header(HttpSocket& sk);
    void recv_chunk(HttpSocket& sk);
};

#endif

void httpBase::recv_chunk(HttpSocket& sk){
    std::string data;
    while(1){
        char buffer[2] = {0};
        sk.recv_msg(buffer, 1, 0);

        data.push_back(buffer[0]);
        payload.push_back(buffer[0]);

        unsigned long length = 0;

        // check for length.
        if(buffer[0] == '\n'){ // finish reciving.
            std::size_t pos = data.find_first_of(" \r");
            std::string lenstr = data.substr(0, pos);
            length = strtoul(lenstr.c_str(), NULL, 16);

            if(length == 0){ // finish reading.
                return;
            }
            else{ // storing data into payload.
                char* contentbuf = new char[length];
                memset(reinterpret_cast<void*>(contentbuf), 0, length);

                sk.recv_msg(contentbuf, length+2, MSG_WAITALL);
                payload.append(contentbuf, length);

                data = "";
            }
        }
    }
}