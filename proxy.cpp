#include "httprequest.h"
#include "httpresponse.h"

#include "proxy.h"
#include "log.h"


// make the server up and listen.
void Proxy::compose_up(){
    try{
        im_server_sk.listen_to(10000);
    }
    catch(...){
        throw;
    }
}


// accept client, and connect.
// HttpSocket Proxy::accept_client(){
int Proxy::accept_client(){
    int client_fd;
    try{
        client_fd = im_server_sk.accept_connect();
    }
    catch(...){
        throw;
    }

    return client_fd;
}

// receive a request from ...
HttpRequest Proxy::recv_request_from(HttpSocket sk){
    HttpRequest this_request;
    try{
        this_request.receive(sk);
    }
    catch(...){
        // re-throw.
        throw;
    }

    return this_request;
}

// receive a response from ...
HttpResponse Proxy::recv_response_from(HttpSocket sk){
    HttpResponse this_response;
    try{
        this_response.receive(sk);
    }
    catch(...){
        // re-throw.
        throw;
    }
    return this_response;
}

//handle all request
void Proxy::handle_request(HttpRequest &request, HttpSocket &server, HttpSocket &client, cache &mycache){
    HttpResponse response;
    if(request.get_method()=="GET"){

      if(request.can_store()){
        try{
            response = mycache.returndata(server, request);
            response.send(client);
        }
        catch(...){
            throw std::exception();
        }
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
        std::cout<<" -------- GET -------- "<<std::endl;
    }
    else if(request.get_method()=="CONNECT"){
        std::cout<<" -------- CONNECT -------- "<<std::endl;
        try{
            request.connect(server, client);
        }
        catch(...){
            throw std::exception();
        }
    }
    else if(request.get_method()=="POST"){
        std::cout<<" -------- POST -------- "<<std::endl;
        try{
            request.send(server);
            // recv from server
            response.receive(server);
            // send to client
            response.send(client);
        }
        catch(...){
            // return;
            throw std::exception();
        }
    }
    else{
        //throw
        request.send_400_bad_request(client);
        throw std::invalid_argument("invalid method.");
    }
}

void Proxy::handle(int client_fd, cache& cache){
    std::cout<<" ----- Handling Client_FD = "<<client_fd<<" -----"<<std::endl;
    std::cout<<"  ---- In Thread "<<std::this_thread::get_id()<<" ---- "<<std::endl;
    
    HttpSocket client_sk;
    HttpSocket server_sk;
    HttpRequest this_request;

    Log log();

    log.output("=== BEGIN Recv Request From Client_FD: " + client_fd + " in Thread: " + std::this_thread::get_id() + " ===");
    try{
        client_sk = HttpSocket(client_fd);
        this_request = recv_request_from(client_sk);
        log.output("=== SUCCESS Recv Request From Client_FD: " + client_fd + " in Thread: " + std::this_thread::get_id() + " ===");
    }
    catch(...){
        // LOG err.
        close(client_fd);
        log.output("=== FAILED Recv Request From Client_FD: " + client_fd + " in Thread: " + std::this_thread::get_id() + " ===");
        return;
    }


    try{
        std::cout<<" ---- Connect to Port: "<<this_request.get_port()<<", Host: "<<this_request.get_host()<<" ---- "<<std::endl;
        server_sk = HttpSocket(this_request.get_port().c_str(), this_request.get_host().c_str());

        handle_request(this_request, server_sk, client_sk, cache);    
    }
    catch(...){
        this_request.send_502_bad_gateway(client_sk);
        return;
    }
}

// std::thread Proxy::create_thread(int client_fd, cache cache){
//     return std::thread(&Proxy::handle, this, client_fd, cache);
// }

