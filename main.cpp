#include <string>
#include <vector>
#include <thread>

#include "proxy.h"
#include "cache.h"
#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"


int main(){
  Proxy proxy;
  cache cache;

  proxy.compose_up();

  while(1){
    // HttpSocket im_server_sk("12345");
    // im_server_sk.listen_to(100);

    // int client_fd = im_server_sk.accept_connect();
    // HttpSocket client_sk(client_fd);

    // // recv from client
    // HttpRequest this_request;
    // this_request.receive(client_sk);

    // HttpSocket client_sk = proxy.accept_client();
    int client_fd = proxy.accept_client();

    // TO-DO : handle pthread here.
    std::thread th(&Proxy::handle, &proxy, client_fd, std::ref(cache));
    
    // std::thread th = proxy.create_thread(client_fd, cache);

    th.detach();
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

  return EXIT_SUCCESS;
}
