#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

int main(){
    HttpSocket im_server_sk("12345");
    server_sk.listen_to(100);
    int client_fd=server_sk.accept_connect();
    HttpSocket client_sk(client_fd);
    HttpRequest this_request;
    //recv from client
    this_request.receive(client_sk);
    HttpSocket server_slk(this_request.get_port(),this_request.get_host());
    HttpResponse this_response;
    //send to server
    this_request.send(server_slk);
    //recv frm server
    this_response.receive(server_slk);
    //send to client
    this_response.send(client_sk);

}
