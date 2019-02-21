#include "httprequest.h"
#include <exception>

void HttpRequest::receive(HttpSocket& sk) {
    int id = 0;
    try {
        id = recv_header(sk);
    }
    catch (...) {
        send_400_bad_request(sk);
        return;
    }

    if(meta[0] == "POST"){
        if(meta[2] == "HTTP/1.0")
            recv_http_1_0(sk);
        if(meta[2] == "HTTP/1.1")
            recv_http_1_1(sk, id);
        else
            throw std::invalid_argument("invlalid protocol.");
    }

}
