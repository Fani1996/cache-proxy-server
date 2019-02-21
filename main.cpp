#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

main(){
    HttpSocket server_sk("50010");
    server_sk.listen_to(100);
    server_sk.accept_connect();
    HttpRequest this_request();
    this_request.receive(server_sk);
}