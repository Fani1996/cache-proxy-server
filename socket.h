#ifndef _SOCKET__H
#define _SOCKET__H
#include <sys/socket.h>
#include <sys/types.h>

class socket{
private:
  int fd;
  int opt;
public:
  socket(const char * port);
  socket(const char * port,const char *hostname);

  // build server.
  void create_as_server(const char * port);
  void create_as_client(const char * port,const char *hostname);

  // listen whether client has connected.
  void listen_to(int nums_client);
  int accept_connect();

  // send/rec info.
  int send_msg(void *info, size_t size);
  int recv_msg(void *info, size_t size, int flag);
  //get_socket_fd();
  
  ~socket();  
};

#endif
