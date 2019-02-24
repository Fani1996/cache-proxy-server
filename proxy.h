#ifndef _PROXY__H
#define _PROXY__H

#include <unistd.h>
#include <time.h>

#include <string>
#include <algorithm>
#include <iostream>
#include <list>
#include <iterator>
#include <utility> 
#include <unordered_map>

#include "httprequest.h"
#include "httpresponse.h"
#include "httpsocket.h"
#include "cache.h"

class proxy{
 public:
 void handle_request(HttpRequest &request,HttpSocket &server,HttpSocket &client, cache &mycache);
 private:




};

#endif
