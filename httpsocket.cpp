#include <stdio.h>
#include "httpsocket.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <cstring>
#include <sys/socket.h>
#include <string.h>
using namespace std;

HttpSocket::HttpSocket(const char * port){
  create_as_server(port);
  
}
HttpSocket::HttpSocket(const char * port, const char *hostname){
  create_as_client(port, hostname);
}
HttpSocket::HttpSocket(int sk_fd):fd(sk_fd){

}

void HttpSocket::create_as_server(const char * port){
  int status;

  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_CANONNAME;
  status = getaddrinfo("0.0.0.0", port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    // TO-DO: throw.
    return;
  } 

  fd = socket(host_info_list->ai_family,host_info_list->ai_socktype,host_info_list->ai_protocol);
  if (fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    // TO-DO: throw.
    return;
  } 

  opt= 1;
 
  status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
  status = bind(fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    fprintf(stderr,"error:%s\n",strerror(errno));
    cerr << "Error: cannot bind socket" << endl;
    // TO-DO: throw.
    return;
  }
  freeaddrinfo(host_info_list);
}
void HttpSocket::create_as_client(const char * port,const char *hostname){
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(hostname, port, &host_info, &host_info_list) != 0) {
    cerr<<"Cannot get address info for host\n"<<endl;
    // TO-DO: throw
    return;
  } 

  fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
  if (fd == -1) {
    // TO-DO: throw
    return;
  }
 
  if (connect(fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
    freeaddrinfo(host_info_list);
    close(fd);
    // TO-DO: throw.
    return;
  }
  freeaddrinfo(host_info_list);
}

int HttpSocket::accept_connect() {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_fd= accept(fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if(client_fd==-1){
    //throw acceptError;
     return -1;
   }
  return client_fd;
}

void HttpSocket::listen_to(int nums_client) {
  if (listen(fd, nums_client) == -1)
  {
    cerr << "Cannot listen on socket." << endl;
    // TO-DO: throw.
    return;
  }
}

int HttpSocket::send_msg(void *info,size_t size) {
  int actual_byte;
  if((actual_byte = send(fd, info, size, 0)) == -1){
    perror("");
    std::cerr<<"Cannot send."<<std::endl;
  }
  return actual_byte;
}
int HttpSocket::recv_msg(void *info,size_t size,int flag) {
  int actual_byte;
  if((actual_byte = recv(fd, info, size, flag)) == -1){
     fprintf(stderr,"error:%s\n",strerror(errno));
    std::cerr<<"Cannot receive."<<std::endl;
    //throw recvError();
  }
  return actual_byte;
}

HttpSocket::~HttpSocket() {
  close(fd);
}
