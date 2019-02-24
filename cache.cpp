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


//TO DO:
//if response exist, update it
//if not exist, add it to cache
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



// given request, return the response ready to send back.
HttpResponse cache::returndata(HttpSocket& server,HttpRequest &request){
    std::string identifier = request.get_identifier();
    if(lookup.find(identifier) != lookup.end()){ // found request id.
        // if found, check valid and no_cache.
       HttpResponse response_in=get(identifier);
       if(response_in.is_fresh()&&!no_cache(request,response_in)){ // valid, return response.
            return response_in;
        }
	else{
	  return revalidate(server,request);
	}
    }
    
    // otherwise, we have to fetch data from server and store in cache.
    HttpResponse response;
    request.send(server);
    response.receive(server);
    //if request no_store and not sutisfied by cache, the new response can not be store
    if(request.can_store()&&response.can_store())
      store(request, response);
    return response;
}



bool cache::no_cache(HttpRequest& httprequest, HttpResponse& httpresponse){
  return httprequest.no_cache()||httprequest.no_cache();
}  

// request to revalidate the data.
HttpResponse cache::revalidate(HttpSocket& server, HttpRequest& request){
    HttpResponse response = get(request.get_identifier());

    // use reponse to create revalidate request to server.
    if (response.get_header_kv("ETag") != "") {
        request.set_header_kv("If-None-Match", response.get_header_kv("ETag"));
        //request.update_header("If-None-Match: " + response.get_header_kv("ETag"));
    }

    if (response.get_header_kv("Last-Modified") != "") {
        request.set_header_kv("If-Modified-Since", response.get_header_kv("Last-Modified"));
        //request.update_header("If-Modified-Since: " + response.get_header_kv("Last-Modified"));
    }
    request.refresh();

    server.send_msg(&request.get_content().data()[0], request.get_content().size());
    HttpResponse revalidate_response;
    revalidate_response.receive(server);

    //reuqest time
    time_t rawtime;
    struct tm * ptm;

    time ( &rawtime );

    ptm = gmtime ( &rawtime );
    time_t request_time = mktime(ptm);
    
    revalidate_response.calculate_initial_age(request_time);

    if(revalidate_response.get_code()=="200"){
      if(revalidate_response.can_store())
	 store(request, revalidate_response);
      return revalidate_response;
    }
    else if(revalidate_response.get_code()=="304"){
      //把revalidate_response的更新过的头给response??
      if(response.can_store())
	store(request, response);
      return response;
    }
    else{
    //throw
    }
    return response;

}
