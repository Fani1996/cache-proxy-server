#include <string>
#include <vector>
#include <thread>

#include "proxy.h"
#include "cache.h"
#include "httpsocket.h"
#include "httpbase.h"
#include "httprequest.h"
#include "httpresponse.h"

unsigned long httpBase::id = 0;

int main(){
	Proxy proxy;
	cache cache;

	try{
		proxy.compose_up();
	}
	catch(...){
		return EXIT_FAILURE;
	}

	while(1){

		int client_fd;
		try{
			client_fd = proxy.accept_client();
		}
		catch(...){
			// try again, and attempt to create new thread.
			continue;
		}

		std::thread th(&Proxy::handle, &proxy, client_fd, std::ref(cache));
		th.detach();

	}

	return EXIT_SUCCESS;
}
