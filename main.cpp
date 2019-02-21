#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

int main(){
<<<<<<< HEAD
    HttpSocket server_sk("12345");
    server_sk.listen_to(100);
    int client_fd=server_sk.accept_connect();
    HttpSocket client_sk(client_fd);
    HttpRequest this_request;

    std::cout<<"enter recieve"<<std::endl;
    this_request.receive(client_sk);
=======
    HttpSocket server_sk("50010");
    server_sk.listen_to(100);
    server_sk.accept_connect();

    HttpRequest this_request;
    this_request.receive(server_sk);

    return 0;
>>>>>>> 7a236e3d6747dfbddd63adb87e61ae38c9f64176
}
