#ifndef _CACHE__H 
#define _CACHE__H

#include <unistd.h>
#include <time.h>

#include <string>
#include <algorithm>
#include <iostream>
#include <list>
#include<iterator>
#include <utility> 
#include <unordered_map>


// cache class, base on LRU.
class cache {
protected:
    // uid, Response
    std::unordered_map<std::string, std::list<std::pair<std::string, Response>>::iterator> lookup;
    std::list<std::pair<std::string, Response>> dataset;

    // given httpResponse, update the data from server.
    void update(Request request);
    // given string, check cache has data / time has expired.
    bool check(Request request);

    // revalidate the data.
    httpResponse revalidate(socketkeeper & server_sock,
                            ttpRequest & info,
                            std::unordered_map<std::string, httpResponse>::iterator to_valid);
public:

    // given request, return the response ready to send back.
    Response returndata(Request request);
};

#endif
