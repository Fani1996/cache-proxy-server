#include <unistd.h>
#include <time.h>

#include <string>
#include <algorithm>
#include <iostream>
#include <list>
#include <iterator>
#include <utility> 
#include <unordered_map>

#include "proxy.h"


//handle all request
void proxy::handle_request(HttpRequest &request,HttpSocket &server,HttpSocket &client, cache &mycache){
  HttpResponse response;
  if(request.get_method()=="GET"){
    response=mycache.returndata(server,request);
    response.send(client);
  }
  else if(request.get_method()=="CONNECT"){
    request.connect(server,client);
  }
  else if(request.get_method()=="POST"){
    request.send(server);
    // //recv frm server
    response.receive(server);
    //send to client
    response.send(client);
  }
  else{
    //throw
  }

}
