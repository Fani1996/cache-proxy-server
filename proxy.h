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
#include <thread>
#include <unordered_map>

#include "httpsocket.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpsocket.h"
#include "cache.h"

class Proxy{
private:
    HttpSocket im_server_sk;
public:
    void handle_request(HttpRequest &request,HttpSocket &server,HttpSocket &client, cache &mycache);

    Proxy() : im_server_sk("12345") {}

    // make the server up and listen.
    void compose_up();
    // accept client, and connect.
    // HttpSocket accept_client();
    int accept_client();

    HttpRequest recv_request_from(HttpSocket sk);
    HttpResponse recv_response_from(HttpSocket sk);
    // void send_request_to(HttpSocket sk, HttpRequest req);

    // void handle(HttpSocket& client_sk, cache& cache);
    void handle(int client_fd, cache& cache);
    // std::thread create_thread(HttpSocket client_sk, cache cache);
};

#endif
