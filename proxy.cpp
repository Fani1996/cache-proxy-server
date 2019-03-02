#include <iostream>
#include <string>


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
    Log log;

    if(request.get_method()=="GET"){
        std::cout<<" -------- GET -------- "<<std::endl;
        if(request.can_store()){
            try{
                response = mycache.returndata(server, request);

                log.output(request.get_id(), "GET " + request.get_host() + " from " + std::to_string(server.get_fd()) + "\n" );
                log.timestamp(request.get_id());

                response.send(client);
            }
            catch(...){
                log.output(request.get_id(), "[GET] EXCEPTION with " + request.get_identifier() + "\n");
                log.timestamp(request.get_id());

                throw std::exception();
            }
        }
        else{
            try{
                log.output(request.get_id(), "not cacheable.\n");
                log.timestamp(request.get_id());

                // send to server
                request.send(server);

                // recv from server
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
        std::cout<<" -------- CONNECT -------- "<<std::endl;
        try{
            request.connect(server, client);
        }
        catch(...){
            log.output(request.get_id(),"CONNECT: Error when communicating with server/client. (Request: " + request.get_identifier() + ")\n");
            log.timestamp(request.get_id());

            throw std::exception();
        }
    }
    else if(request.get_method()=="POST"){
        std::cout<<" -------- POST -------- "<<std::endl;
        try{
            // send to server
            request.send(server);
            
            // recv from server
            response.receive(server);

            // send to client
            response.send(client);
        }
        catch(...){
            // return;
            log.output(request.get_id(), "POST: Error when communicating with server/client. (Request: " + request.get_identifier() + ")\n");
            log.timestamp(request.get_id());

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
    Log log;

    try{
        client_sk = HttpSocket(client_fd);
        this_request = recv_request_from(client_sk);

        std::stringstream trystr;
        trystr << "request " << this_request.get_method()+" "<<this_request.get_identifier()<<" "<<this_request.get_version() << " from " << this_request.get_host() << " ===\n";
        log.output(this_request.get_id(), trystr.str());
        log.timestamp(this_request.get_id());

        log.log_request(this_request.get_id(), this_request);
    }
    catch(...){
        // LOG err.
        close(client_fd);
        std::stringstream errstr;
        errstr << "=== FAILED Recv Request From Client_FD: " << std::to_string(client_fd) << " in Thread: " << std::this_thread::get_id() << " ===\n";
        log.output(this_request.get_id(), errstr.str());
        //        log.log_request(this_request);
        log.timestamp(this_request.get_id());

        return;
    }

    try{
        std::cout<<" ---- Connect to Port: "<<this_request.get_port()<<", Host: "<<this_request.get_host()<<" ---- "<<std::endl;
        server_sk = HttpSocket(this_request.get_port().c_str(), this_request.get_host().c_str());

        handle_request(this_request, server_sk, client_sk, cache);
        close(server_sk.get_fd());
        close(client_sk.get_fd());
    }
    catch(...){
        this_request.send_502_bad_gateway(client_sk);

        std::stringstream errstr;
        errstr << "=== FAILED Connect Server/Handle Request: " << " in Thread: " << std::this_thread::get_id() << " ===\n";
        log.output(this_request.get_id(), errstr.str());
        //log.log_request(this_request);
        log.timestamp(this_request.get_id());
        return;
    }
}
