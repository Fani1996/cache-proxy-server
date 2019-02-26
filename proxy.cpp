#include "httprequest.h"
#include "httpresponse.h"

#include "proxy.h"

// make the server up and listen.
void Proxy::compose_up(){
    im_server_sk.listen_to(10000);
}


// accept client, and connect.
// HttpSocket Proxy::accept_client(){
int Proxy::accept_client(){
    int client_fd = im_server_sk.accept_connect();
    // HttpSocket client_sk(client_fd);

    // return client_sk;
    return client_fd;
}

// receive a request from ...
HttpRequest Proxy::recv_request_from(HttpSocket sk){
    HttpRequest this_request;
    try{
        this_request.receive(sk);
    }
    catch(...){
        throw;
    }

    return this_request;
}

// receive a response from ...
HttpResponse Proxy::recv_response_from(HttpSocket sk){
    HttpResponse this_response;
    this_response.receive(sk);

    return this_response;
}

//handle all request
void Proxy::handle_request(HttpRequest &request, HttpSocket &server, HttpSocket &client, cache &mycache){
    HttpResponse response;
    if(request.get_method()=="GET"){
      if(request.can_store()){
        response = mycache.returndata(server,request);
        response.send(client);
      }
      else{
	request.send(server);
        // recv from server
        try{
            response.receive(server);
        }
        catch(...){
            return;
        }
        // send to client
        response.send(client);
      }
    }
    else if(request.get_method()=="CONNECT"){
        request.connect(server,client);
    }
    else if(request.get_method()=="POST"){
        request.send(server);
        // recv from server
        try{
            response.receive(server);
        }
        catch(...){
            return;
        }
        // send to client
        response.send(client);
    }
    else{
    //throw
    }
}

void Proxy::handle(int client_fd, cache& cache){
    try{
        HttpSocket client_sk(client_fd);

        HttpRequest this_request = recv_request_from(client_sk);

        std::cout<<"port: "<<this_request.get_port()<<", host: "<<this_request.get_host()<<std::endl;
        HttpSocket server_sk(this_request.get_port().c_str(), this_request.get_host().c_str());
        
        handle_request(this_request, server_sk, client_sk, cache);
    }
    catch(...){
        close(client_fd);
        return;
    }
}

// std::thread Proxy::create_thread(int client_fd, cache cache){
//     return std::thread(&Proxy::handle, this, client_fd, cache);
// }

