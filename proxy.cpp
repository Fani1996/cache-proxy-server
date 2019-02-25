#include "httprequest.h"
#include "httpresponse.h"

#include "proxy.h"

// make the server up and listen.
HttpSocket Proxy::compose_up(){
    im_server_sk.listen_to(100);
}


// accept client, and connect.
HttpSocket Proxy::accept_client(){
    int client_fd = im_server_sk.accept_connect();
    HttpSocket client_sk(client_fd);

    return client_sk;
}

// receive a request from ...
HttpRequest Proxy::recv_request_from(HttpSocket sk){
    HttpRequest this_request;
    this_request.receive(sk);

    return this_request;
}

// receive a response from ...
HttpResponse Proxy::recv_response_from(HttpSocket sk){
    HttpResponse this_response;
    this_response.receive(sk);

    return this_response;
}

void Proxy::send_request_to(HttpSocket sk, HttpRequest req){

}

