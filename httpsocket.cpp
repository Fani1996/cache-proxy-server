#include <iostream>
#include <cstring>
#include <exception>

#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

#include "httpsocket.h"

using namespace std;


// constructor
HttpSocket::HttpSocket(){}
HttpSocket::HttpSocket(const char * port){
    create_as_server(port);
}

HttpSocket::HttpSocket(const char * port, const char *hostname) try {
	create_as_client(port, hostname);
}
catch(...){
  throw std::exception();
}

HttpSocket::HttpSocket(int sk_fd):fd(sk_fd){}

int HttpSocket::get_fd(){
  return fd;
}

// create socket server as server.
void HttpSocket::create_as_server(const char * port){
  int status;

  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_CANONNAME;

  status = getaddrinfo("0.0.0.0", port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    // throw.
    throw std::exception();
  } 

  fd = socket(host_info_list->ai_family,host_info_list->ai_socktype,host_info_list->ai_protocol);
  if (fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    // throw.
    throw std::exception();
  } 

  opt= 1;
  status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
  status = bind(fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    fprintf(stderr,"error:%s\n",strerror(errno));
    cerr << "Error: cannot bind socket" << endl;
    // throw.
    throw std::exception();
  }
  //freeaddrinfo(host_info_list);
}

void HttpSocket::create_as_client(const char * port,const char *hostname){
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(hostname, port, &host_info, &host_info_list) != 0) {
    cerr<<"Cannot get address info for host\n"<<endl;
    // throw
    throw std::exception();
  } 

  fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
  if (fd == -1) {
    // throw
    throw std::exception();
  }

  if (connect(fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
    // freeaddrinfo(host_info_list);
    //close(fd);
    // throw.
    throw std::exception();
  }
  //freeaddrinfo(host_info_list);
}

int HttpSocket::accept_connect() {
	struct sockaddr_storage socket_addr;
	socklen_t socket_addr_len = sizeof(socket_addr);
	
	int client_fd= accept(fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
	if(client_fd==-1){
		throw std::exception();
	}
	return client_fd;
}

void HttpSocket::listen_to(int nums_client) {
	if (listen(fd, nums_client) == -1){
		cerr << "Cannot listen on socket." << endl;
		// throw.
		throw std::exception();
	}
}

int HttpSocket::send_msg(void *info,size_t size) {
	int actual_byte;
	if((actual_byte = send(fd, info, size, 0)) == -1){
		perror("");
		std::cerr<<"Cannot send."<<std::endl;
    throw std::exception();
	}
	return actual_byte;
}

int HttpSocket::recv_msg(void *info,size_t size,int flag) {
	int actual_byte;
	if((actual_byte = recv(fd, info, size, flag)) == -1){
		fprintf(stderr,"error:%s\n",strerror(errno));
		std::cerr<<"Cannot receive."<<std::endl;
		throw std::exception();
	}
	return actual_byte;
}

HttpSocket::~HttpSocket() {
  //close(fd);
}
