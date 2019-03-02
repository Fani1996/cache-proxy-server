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
#include <mutex>

#include "httprequest.h"
#include "httpresponse.h"



// cache class, base on LRU.
class cache {
protected:
    std::size_t capacity;
    std::mutex mtx;    

    // uid, Response
    std::unordered_map<std::string, std::list<std::pair<std::string, HttpResponse> >::iterator> lookup;
    std::list<std::pair<std::string, HttpResponse> > dataset;

    // LRU operation. get/store will update the position of data.
     // get the response from cache.
    HttpResponse get(std::string identifier);

    // store the request, response pait into cache.
    void store(HttpRequest request, HttpResponse response);

    // request to revalidate the data.
    HttpResponse revalidate(HttpSocket& server, HttpRequest& request);

public:
    cache() : capacity(256) {}
    cache(std::size_t cap) : capacity(cap) {}

    // check if the request have no cache.
    bool no_cache(HttpRequest &httprequest,HttpResponse &httpresponse);

    // given request, return the response ready to send back.
    HttpResponse returndata(HttpSocket& server,HttpRequest &request);
    
};

#endif
