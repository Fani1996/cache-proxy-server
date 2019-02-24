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

    // return identifier for request to id the request in cache.
    std::string get_identifier();
    // return request's method.
    std::string get_method();
    // return request's version.
    std::string get_version();
    
    // get hostname from request.
    std::string get_host();
    // get port from request.
    std::string get_port();

    // deal with CONNECT method of the request.
    void connect(HttpSocket& server, HttpSocket& client);
    // receive request.
    void receive(HttpSocket& sk);

};

#endif
