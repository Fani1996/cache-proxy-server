#include <string>
#include <vector>

#include "proxy.h"
#include "cache.h"
#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

int main(){
  while(1){
    // HttpSocket im_server_sk("12345");
    // im_server_sk.listen_to(100);

    // int client_fd = im_server_sk.accept_connect();
    // HttpSocket client_sk(client_fd);

    // // recv from client
    // HttpRequest this_request;
    // this_request.receive(client_sk);

    Proxy proxy;
    cache cache;

    proxy.compose_up();

    HttpSocket client_sk = proxy.accept_client();
    // TO-DO : handle pthread here.
    HttpRequest this_request = proxy.recv_request_from(client_sk);

    std::cout<<"port: "<<this_request.get_port()<<", host: "<<this_request.get_host()<<std::endl;
    HttpSocket server_sk(this_request.get_port().c_str(), this_request.get_host().c_str());
    
    proxy.handle_request(this_request, server_sk, client_sk, cache);
    // if(this_request.get_method()=="CONNECT"){
    //   std::cout<<"this is connect"<<std::endl;
    //   this_request.connect(server_sk,client_sk);
    //   break;
    // }

    
    // std::cout<<"\n=== to server ==="<<std::endl;
    // //send to server
    // this_request.send(server_sk);


    // //recv frm server
    // std::cout<<"\n=== receiving from server ==="<<std::endl;
    
    // HttpResponse this_response;
    // this_response.receive(server_sk);
    // HttpResponse this_response = proxy.recv_response_from(server_sk);

    //send to client
    // this_response.send(client_sk);
  }
}
