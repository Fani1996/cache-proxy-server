#include "httpresponse.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <unordered_map>
#include <string>
#include <vector>
#include "httpsocket.h"
#include "httpbase.h"

void HttpResponse::receive(HttpSocket &sk){
    int recv_type=recv_header(sk);
    if((meta[1]!= "304"&&meta[1]!="204"&&meta[1]!="205"){
        if(meta[0]=="HTTP/1.0"){
            recv_http_1_0(sk);
        }
        else if(meta[0]=="HTTP/1.1")
        {
            recv_http_1_1(sk,recv_type);
        }
        else{
            std::cerr<<"Invalid Version."<<std::endl;
            //throw
        }
    }
    
}
