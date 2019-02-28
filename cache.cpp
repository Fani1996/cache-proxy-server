#include "cache.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>



// get the response from cache.
HttpResponse cache::get(std::string identifier){
        std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
             lck.lock();

    HttpResponse response = lookup[identifier]->second;
    dataset.splice(dataset.begin(), dataset, lookup[identifier]); // send to front.
    
      lck.unlock();

    return response;
}

// store the request, response pait into cache.
void cache::store(HttpRequest request, HttpResponse response){
    std::unique_lock<std::mutex> lck(mtx, std::defer_lock);

    std::string id = request.get_identifier();
    // key exists.
    if(lookup.find(id) != lookup.end()){
        lck.lock();

        lookup[id]->second = response;
        dataset.splice(dataset.begin(), dataset, lookup[id]); // send to front.

	  lck.unlock();
	
        return;
    }

    // key not exist, check size before insert.
    if(dataset.size() > capacity){
        std::string erasekey = dataset.back().first;

	        lck.lock();

        dataset.pop_back();

        lookup.erase(erasekey);

	 lck.unlock();
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
		std::cout<<"===find a response in cache==="<<std::endl;
		HttpResponse response_in = get(identifier);
		if(response_in.is_fresh() && !no_cache(request,response_in) && !response_in.must_revalidate()){ // valid, return response.

			std::cout<<"===response is fresh and can cache==="<<std::endl;
			return response_in;
		  }
		else{
			std::cout<<"===need re-validated==="<<std::endl;
			HttpResponse revalidated;
			try{
				revalidated = revalidate(server,request);
			}
			catch(...){
				throw;
			}
			return revalidated;
		}
      }

	std::cout<<"===cannot find response in cache==="<<std::endl;
    // otherwise, we have to fetch data from server and store in cache.
    HttpResponse response;
        try{
	  std::cout<<"send request to server"<<std::endl;
		request.send(server);
	}
	catch(...){
	  std::cout<<"send request to server fail"<<std::endl;
		throw std::exception();
	}

	try{
	  std::cout<<"receive response from server"<<std::endl;
		response.receive(server);
	}
	catch(...){
		throw std::exception();
	}

    //if request no_store and not sutisfied by cache, the new response can not be store
    if(request.can_store() && response.can_store() && response.get_code() == "200"){
		store(request, response);
    }
    return response;
}



bool cache::no_cache(HttpRequest& httprequest, HttpResponse& httpresponse){
	return httprequest.no_cache() || httprequest.no_cache();
}  

// request to revalidate the data.
HttpResponse cache::revalidate(HttpSocket& server, HttpRequest& request){
    HttpResponse response = get(request.get_identifier());

    // use reponse to create revalidate request to server.
    if (response.get_header_kv("ETag") != "") {
        std::string etag=response.get_header_kv("ETag");
        std::vector<char> etag_char;
        std::copy( etag.begin(), etag.end(), std::back_inserter(etag_char));
        request.set_header_kv({'I','f','-','N','o','n','e','-','M','a','t','c','h'},etag_char);
        //request.update_header("If-None-Match: " + response.get_header_kv("ETag"));
    }
    if (response.get_header_kv("Last-Modified") != "") {
      std::string last_modified=response.get_header_kv("Last-Modified");
        std::vector<char> last_char;
        std::copy( last_modified.begin(), last_modified.end(), std::back_inserter(last_char));
        request.set_header_kv({'I','f','-','M','o','d','i','f','i','e','d','-','S','i','n','c','e'}, last_char);
        //request.update_header("If-Modified-Since: " + response.get_header_kv("Last-Modified"));
    }

    //    request.generate_header();
    // request.refresh();
	
    std::cout<<"=== send re-validated request ==="<<std::endl;
    try{
        server.send_msg(&request.get_content().data()[0], request.get_content().size());
    }
    catch(...){
        throw;
    }
    
	HttpResponse revalidate_response;
	try{
		revalidate_response.receive(server);
	}
	catch(...){
		throw std::exception();
	}

    
    if(revalidate_response.get_code() == "200"){
		std::cout<<"===get 200 from server==="<<std::endl;
		if(revalidate_response.can_store())
			store(request, revalidate_response);
		return revalidate_response;
    }
    else if(revalidate_response.get_code() == "304"){
		std::cout<<"===get 304 from server==="<<std::endl;
		//把revalidate_response的更新过的头给response??
		if(response.can_store())
			store(request, response);
		return response;
    }
    else{
		//throw
		throw std::exception();
    }

    return response;
}
