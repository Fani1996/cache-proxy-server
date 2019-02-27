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

                log.output( "GET: " + request.get_host() + " from " + std::to_string(server.get_fd()) + "\n" );
                log.timestamp();

                response.send(client);
                
                log.output( "GET: Send Response To " + std::to_string(client.get_fd()) + "\n" );
                log.timestamp();
            }
            catch(...){
                log.output( "GET: EXCEPTION with " + request.get_identifier() + "\n");
                log.timestamp();

                throw std::exception();
            }
        }
        else{
            try{
                // send to server
                request.send(server);
                                
                log.output("GET: [Cannot Store] Send to Server for Request: " + request.get_identifier() + "\n");
                log.timestamp();

                // recv from server
                response.receive(server);

                log.output("GET: [Cannot Store] Receive Response from Server for Request: " + request.get_identifier() + "\n");
                log.timestamp();
            }
            catch(...){
                log.output("GET: [Cannot Store] Error when communicating with server. (Request: " + request.get_identifier() + ")\n");
                log.timestamp();

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
            log.output("CONNECT: Error when communicating with server/client. (Request: " + request.get_identifier() + ")\n");
            log.timestamp();

            throw std::exception();
        }
    }
    else if(request.get_method()=="POST"){
        std::cout<<" -------- POST -------- "<<std::endl;
        try{
            // send to server
            request.send(server);

            log.output("POST: Send to Server for Request: " + request.get_identifier() + "\n");
            log.timestamp();
            
            // recv from server
            response.receive(server);

            log.output("POST: Receive Response from Server for Request: " + request.get_identifier() + "\n");
            log.timestamp();

            // send to client
            response.send(client);

            log.output("POST: Send Response to Client for Request: " + request.get_identifier() + "\n");
            log.timestamp();
        }
        catch(...){
            // return;
            log.output("POST: Error when communicating with server/client. (Request: " + request.get_identifier() + ")\n");
            log.timestamp();

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

    std::stringstream ss;
    ss << "=== BEGIN Recv Request From Client_FD: " << std::to_string(client_fd) << " in Thread: " << std::this_thread::get_id() << " ===\n";
    log.output(ss.str());

    try{
        client_sk = HttpSocket(client_fd);
        this_request = recv_request_from(client_sk);

        std::stringstream trystr;
        trystr << "=== SUCCESS Recv Request From Client_FD: " << client_fd << " in Thread: " << std::this_thread::get_id() << " ===\n";
        log.output(trystr.str());
        // log.log_request(this_request);
        log.timestamp();
    }
    catch(...){
        // LOG err.
        close(client_fd);
        std::stringstream errstr;
        ss << "=== FAILED Recv Request From Client_FD: " << std::to_string(client_fd) << " in Thread: " << std::this_thread::get_id() << " ===\n";
        log.output(errstr.str());
        // log.log_request(this_request);
        log.timestamp();

        return;
    }

    try{
        std::cout<<" ---- Connect to Port: "<<this_request.get_port()<<", Host: "<<this_request.get_host()<<" ---- "<<std::endl;
        server_sk = HttpSocket(this_request.get_port().c_str(), this_request.get_host().c_str());

        handle_request(this_request, server_sk, client_sk, cache);
    }
    catch(...){
        this_request.send_502_bad_gateway(client_sk);

        std::stringstream errstr;
        ss << "=== FAILED Connect Server/Handle Request: " << " in Thread: " << std::this_thread::get_id() << " ===\n";
        log.output(errstr.str());
        // log.log_request(this_request);
        log.timestamp();
        return;
    }
}
