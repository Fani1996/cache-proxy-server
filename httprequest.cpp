#include "httprequest.h"
#include <exception>
#include <algorithm>


std::string HttpRequest::get_method(){
  return meta[0];
}
// get hostname from request.
std::string HttpRequest::get_host() {
    std::string host;
    if(headerpair.count("Host") == 0) { // host not exist.
        return hostname = host;
    }
    
    host = headerpair["Host"];
    std::size_t pos = headerpair["Host"].find_first_of(':');
    if(pos == std::string::npos)
        hostname = host;
    else {
        hostname = host.substr(0, pos);
        port = host.substr(pos+1);
    }

    hostname.erase(std::remove(hostname.begin(), hostname.end(), ' '), hostname.end());
    return hostname;
}

// get port from request.
std::string HttpRequest::get_port() {
    std::size_t pos;
    if(headerpair.find("Host") != headerpair.end()){
        if( (pos = headerpair["Host"].find_first_of(':')) != std::string::npos ){
            return port = headerpair["Host"].substr(pos+1);
        }
    }

    // using meta to determine port.
    if(meta.size() == 3){
        if(meta[2].find("HTTPS") != std::string::npos){
            port = "443";
        }
        else if(meta[2].find("HTTP") != std::string::npos){
            port = "80";
        }
    }

  return port;
  // other situation will make the port empty string, we can check empty for exception.
}

// return request's method.
std::string HttpRequest::get_method(){
    return meta[0];
}

// return identifier for request to id the request in cache.
std::string HttpRequest::get_identifier(){
    return meta[1];
}

// return request's version.
std::string HttpRequest::get_version(){
    return meta[2];
}

// deal with CONNECT method of the request.
void HttpRequest::connect(HttpSocket& server, HttpSocket& client){
    std::string response(get_version() + " 200 OK\r\n\r\n");
    std::cout<<"CONNECT Response: "<<response<<std::endl;

    client.send_msg(const_cast<char *>(response.c_str()), response.size());

    // connect_blind_transmit
}

// receive request.
void HttpRequest::receive(HttpSocket& sk) {
    int id = 0;
    try {
        id = recv_header(sk);
    }
    catch (...) {
        send_400_bad_request(sk);
        return;
    }

    if(meta[0] == "POST"){
        if(meta[2] == "HTTP/1.0")
            recv_http_1_0(sk);
        if(meta[2] == "HTTP/1.1")
            recv_http_1_1(sk, id);
        else
            throw std::invalid_argument("invlalid protocol.");
    }

}

