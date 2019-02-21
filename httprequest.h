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

    // get hostname from request.
    void get_host();
    // get port from request.
    void get_port();

public:
    // return identifier for request to id the request in cache.
    std::string get_identifier();
    // receive request.
    void receive(HttpSocket& sk);
};

#endif
