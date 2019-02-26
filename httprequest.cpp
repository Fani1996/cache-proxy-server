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

    try{
        client.send_msg(const_cast<char *>(response.c_str()), response.size());
    }
    catch(...){
        send_502_bad_gateway(server);
        return;
    }

    int client_fd = client.get_fd();
    int server_fd = server.get_fd();

    int active;
    //connect server and client
    while(1){
        fd_set read_fd;
        FD_ZERO(&read_fd);

        FD_SET (client_fd, &read_fd);
        FD_SET (server_fd, &read_fd);

        int max_fd = client_fd > server_fd ? client_fd : server_fd;
	
    // struct timeval timeout;
	// timeout.tv_sec = 1;
	// timeout.tv_usec = 500000;//500ms
	
        // active = select(max_fd+1, &read_fd, NULL, NULL, &timeout);
        int active = select(max_fd+1, &read_fd, NULL, NULL,NULL);
	if(active==0){
	  break;
	}
        else{
	  //            std::cout<<"select active\n"<<std::endl;
            if(FD_ISSET(client_fd, &read_fd)){
	      // std::cout<<"=== CLIENT Active ==="<<std::endl;
                std::vector<char> buffer(10000,0);

                int actual_byte = client.recv_msg(&buffer.data()[0], 10000, 0);
                buffer.resize(actual_byte);
                //std::cout<<"actual byte"<<actual_byte<<std::endl;
                //if connect closed
                if(actual_byte == 0){
                    break;
                }

                try{
                    server.send_msg(&buffer.data()[0], actual_byte);
                }
                catch(...){
                    send_502_bad_gateway(client);
                    return;
                }
            }
            else if(FD_ISSET(server_fd, &read_fd)){
	      //std::cout<<"=== SERVER Active ==="<<std::endl;
                std::vector<char> buffer(10000,0);

                int actual_byte;
                try{
                    actual_byte = server.recv_msg(&buffer.data()[0], 10000, 0);
                    buffer.resize(actual_byte);
                }
                catch(...){
                    close(client_fd);close(server_fd);  break;
                }
                //std::cout<<"actual byte"<<actual_byte<<std::endl;
                //if connect close
                if(actual_byte == 0){
                    break;
                }

                try{
                    client.send_msg(&buffer.data()[0], actual_byte);
                }
                catch(...){
                    send_502_bad_gateway(server);
                    close(client_fd);close(server_fd);  break;
                }
            }
        }
    }
            // if(active == 0){
			// 	std::cout<<"Tunnel closed"<<std::endl;
			// 	close(server_fd);	
			// }
}

// receive request.
void HttpRequest::receive(HttpSocket& sk) {
    int id = 0;
    try {
        id = recv_header(sk);
    }
    catch (...) {
        send_400_bad_request(sk);
        throw;
    }

    if(meta[0] == "POST"){
        if(meta[2] == "HTTP/1.0"){
            try{
                recv_http_1_0(sk);
            }
            catch(...){
                throw;
            }
        }
        if(meta[2] == "HTTP/1.1"){
            try{
                recv_http_1_1(sk, id);
            }
            catch(...){
                throw;
            }
        }
        else{
            send_400_bad_request(sk);
            throw std::invalid_argument("invlalid protocol.");
        }
    }

}

