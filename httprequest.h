#ifndef _HTTPREQUEST__H
#define _HTTPREQUEST__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"

class HttpRequest : public httpBase{
private:
    std::string get_method();
    std::string get_protocol();

public:
    void receive(HttpSocket& sk);
};

#endif
