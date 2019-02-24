#ifndef _CACHE__H 
#define _CACHE__H

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



// cache class, base on LRU.
class cache {
protected:
    std::size_t capacity;
    
    // uid, Response
    std::unordered_map<std::string, std::list<std::pair<std::string, HttpResponse> >::iterator> lookup;
    std::list<std::pair<std::string, HttpResponse> > dataset;

    // LRU operation. get/store will update the position of data.
    // get the response from cache.
    HttpResponse get(std::string identifier);

    // store the request, response pait into cache.
    void store(HttpRequest request, HttpResponse response);

    // Data-related operation.
    // given string, check cache has data / time has expired.
    // bool check(HttpRequest request);
    
    // given httpRequest, update the data from server.
    // HttpResponse update(HttpRequest request);

    // request to revalidate the data.
    HttpResponse revalidate(HttpSocket& server, HttpRequest& request);

public:
    bool can_store(HttpRequest &httprequest,HttpResponse &httpresponse);
    // given request, return the response ready to send back.
    HttpResponse returndata(HttpRequest request);

};

#endif
