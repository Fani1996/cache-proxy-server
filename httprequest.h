#ifndef _HTTPREQUEST__H
#define _HTTPREQUEST__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"

class HttpRequest : public httpBase {
private:
    std::string hostname;
    std::string port;


public:
    // constructor.
    HttpRequest(){}
    HttpRequest(std::string ct) : httpBase(ct){}

    // return identifier for request to id the request in cache.
    std::string get_identifier();
    // receive request.
    void receive(HttpSocket& sk);
    // get hostname from request.
    std::string get_host();
    // get port from request.
    std::string get_port();
    std::string get_method();
};

#endif
