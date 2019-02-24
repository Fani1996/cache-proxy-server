#include "httprequest.h"
#include <exception>
#include <algorithm>

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

    int client_fd=client.get_fd();
    int server_fd=server.get_fd();
    //connect server and client
    while(1){
      fd_set read_fd;
      FD_ZERO(&read_fd);
      int max_fd=client_fd > server_fd ? client_fd:server_fd;
      FD_SET (client_fd, &read_fd);
      FD_SET (server_fd, &read_fd);
      int active=select(max_fd+1,&read_fd,NULL,NULL,NULL);
      if(active>0){
	std::cout<<"select active\n"<<std::endl;
        if(FD_ISSET(client_fd, &read_fd)){
	  std::cout<<"client active!!!"<<std::endl;
	  std::vector<char> buffer(8192,0);
	  //memset(&buffer,0,sizeof(buffer));
	  int actual_byte=client.recv_msg(&buffer.data()[0],8192,0);
	  buffer.resize(actual_byte);
	  std::cout<<"actual byte"<<actual_byte<<std::endl;
	  //std::cout<<"buffer::"<<(std::string)(buffer)<<std::endl;
	  //if connect closed
	  if(actual_byte==0||actual_byte==-1)
	    break;
	  server.send_msg(&buffer.data()[0],actual_byte);
	}
	if(FD_ISSET(server_fd, &read_fd)){
	  std::cout<<"server active!!!!"<<std::endl;
	  std::vector<char> buffer(8192,0);
          //memset(&buffer,0,sizeof(buffer));
          int actual_byte=server.recv_msg(&buffer.data()[0],8192,0);
	   buffer.resize(actual_byte);
	  std::cout<<"actual byte"<<actual_byte<<std::endl;
	  // std::cout<<"buffer::"<<(std::string)(buffer)<<std::endl;
	  //if connect close
          if(actual_byte==0||actual_byte==-1)
            break;
          client.send_msg(&buffer.data()[0],actual_byte);
	}
      }
    }
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

