#ifndef _HTTPREQUEST__H
#define _HTTPREQUEST__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"

class HttpRequest : public httpBase{
private:
    std::string hostname;
    std::string port;

    void get_host();
    void get_port();

public:
    void receive(HttpSocket& sk);
};

#endif
