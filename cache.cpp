#include "cache.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>



// get the response from cache.
HttpResponse cache::get(std::string identifier){
    HttpResponse response = lookup[identifier]->second;
    dataset.splice(dataset.begin(), dataset, lookup[identifier]); // send to front.
    
    return response;
}

// store the request, response pait into cache.
void cache::store(HttpRequest request, HttpResponse response){
    std::string id = request.get_identifier();
    // key exists.
    if(lookup.find(id) != lookup.end()){
        lookup[id]->second = response;
        dataset.splice(dataset.begin(), dataset, lookup[id]); // send to front.
        return;
    }

    // key not exist, check size before insert.
    if(dataset.size() > capacity){
        std::string erasekey = dataset.back().first;
        dataset.pop_back();

        lookup.erase(erasekey);
    }
    // put newly added to the front.
    dataset.push_front(make_pair(id, response));
    lookup[id] = dataset.begin();
}



// given string, check cache has data / time has expired.
bool cache::check(HttpRequest request){

}

// given httpRequest, update the data from server.
HttpResponse cache::update(HttpRequest request){

}

// given request, return the response ready to send back.
HttpResponse cache::returndata(HttpRequest request){
    std::string identifier = request.get_identifier();
    if(lookup.find(identifier) != lookup.end()){ // found request id.
        // if found, check valid.
        if(check(request)){ // valid, return response.
            return get(identifier);
        }
    }

    // otherwise, we have to fetch data from server and store in cache.
    HttpResponse response = update(request);
    store(request, response);
    return response;
}
