#ifndef _PROXY__H
#define _PROXY__H

#include <unistd.h>
#include <time.h>

#include <string>
#include <algorithm>
#include <iostream>
#include <list>
#include <iterator>
#include <utility> 
#include <unordered_map>

#include "httpsocket.h"
#include "httprequest.h"
#include "httpresponse.h"

class Proxy {
private:
    HttpSocket im_server_sk;


public:
    Proxy() : im_server_sk("12345") {}

    // make the server up and listen.
    HttpSocket compose_up();
    // accept client, and connect.
    HttpSocket accept_client();

    HttpRequest recv_request_from(HttpSocket sk);
    HttpResponse recv_response_from(HttpSocket sk);
    // void send_request_to(HttpSocket sk, HttpRequest req);

};

#endif
