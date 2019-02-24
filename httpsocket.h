#ifndef _HTTPSOCKET__H
#define _HTTPSOCKET__H
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/socket.h>
#include <string.h>


class HttpSocket{
private:
  int fd;
  int opt;
public:
  HttpSocket(const char * port);
  HttpSocket(int sk_fd);
  HttpSocket(const char * port,const char *hostname);

  int get_fd();
  // build server.
  void create_as_server(const char * port);
  void create_as_client(const char * port,const char *hostname);

  // listen whether client has connected.
  void listen_to(int nums_client);
  int accept_connect();

  // send/rec info.
  int send_msg(void *info, size_t size);
  int recv_msg(void *info, size_t size, int flag);
  
  ~HttpSocket();  
};

#endif
