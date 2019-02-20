#ifndef _CACHE__H 
#define _CACHE__H

#include <unistd.h>
#include <time.h>

#include <string>
#include <unordered_map>


// cache class, base on LRU.
class cache {
private:
    std::unordered_map<std::string, int> dataset;

    // given httpResponse, update the data from server.
    void update();
    // read header.
    std::string cache::read_header(socketkeeper & sock);
    // revalidate the data.
    httpResponse revalidate(socketkeeper & server_sock,
                            ttpRequest & info,
                            std::unordered_map<std::string, httpResponse>::iterator to_valid);
public:
    size_t get_size();
    // given string, check cache has data / time has expired.
    check(std::string request);

};

#endif
