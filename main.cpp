#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

int main(){
    HttpSocket im_server_sk("12345");
    im_server_sk.listen_to(100);
    int client_fd = im_server_sk.accept_connect();

    HttpSocket client_sk(client_fd);
    HttpRequest this_request;
    //recv from client
    this_request.receive(client_sk);

    std::cout<<"port: "<<this_request.get_port()<<", host: "<<this_request.get_host()<<std::endl;
    HttpSocket server_sk(this_request.get_port().c_str(), this_request.get_host().c_str());
    HttpResponse this_response;

    if(this_request.get_method()=="connect"){
      std::cout<<"this is connect"<<std::endl;
      this_request.connect(client_sk,server_sk);
    }

    
    std::cout<<"\n=== to server ==="<<std::endl;
    // //send to server
    this_request.send(server_sk);
    // //recv frm server
    std::cout<<"\n=== receiving from server ==="<<std::endl;
    this_response.receive(server_sk);
    //send to client
    this_response.send(client_sk);
    //this_request.send(client_sk);

}
