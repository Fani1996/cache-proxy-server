#include "httpresponse.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include "httpsocket.h"

void HttpResponse::receive(HttpSocket &sk) {
	int recv_type;
	try{
		recv_type = recv_header(sk);
	}
	catch (...){
		return;
	}

	if (meta[1] != "304" && meta[1] != "204" && meta[1] != "205"){
		if (meta[0] == "HTTP/1.0"){
			try{
				recv_http_1_0(sk);
			}
			catch(...){
				throw;
			}
		}
		else if (meta[0] == "HTTP/1.1"){
			try{
				recv_http_1_1(sk, recv_type);
			}
			catch(...){
				throw;
			}
		}
		else{
			std::cerr << "Invalid Version." << std::endl;
			//throw
			throw std::invalid_argument("invalid version.");
		}
	}
}

std::string HttpResponse::get_code(){
	return meta[1];
}


bool HttpResponse::is_fresh(){
	double fresh_lifetime;
	double current_age = get_current_age();

	std::string temp = get_cache_control("s-maxage");
	if(temp == ""){
		temp = get_cache_control("max-age");
	}
	else{
		fresh_lifetime = std::stod(temp);
	}

	if(temp==""){
		temp=get_header_kv("Expires");
	}
	else{
		fresh_lifetime=std::stod(temp);
	}

	//Heuristic Freshness
	if(temp == ""){
		struct tm last_modified_tm;
		memset(&last_modified_tm, 0, sizeof(struct tm));

		strptime(get_header_kv("Last-Modified").c_str(), "%a, %d %b %Y %H:%M:%S %Z", &last_modified_tm);
		
		time_t last_modified = mktime(&last_modified_tm);   
		fresh_lifetime = difftime(last_modified, time(NULL))*0.1;
	}
	else{
		fresh_lifetime = std::stod(temp)-std::stod(get_header_kv("Date"));
	}

	return fresh_lifetime > current_age;
}

double HttpResponse::get_current_age(){
	double resident_time = difftime(response_time, time(NULL));
	return initial_age + resident_time;
}

void HttpResponse::calculate_initial_age(time_t request_time){
	struct tm data_value_tm;
	memset(&data_value_tm, 0, sizeof(struct tm));

	strptime(get_header_kv("Date").c_str(), "%a, %d %b %Y %H:%M:%S %Z", &data_value_tm);
	
	time_t data_value = mktime(&data_value_tm);
	double apparent_age = difftime(response_time, data_value);
	
	if(apparent_age < 0) apparent_age = 0;
	
	double response_delay = difftime(response_time, request_time);

	double age_value;
	std::string temp = get_header_kv("Age");
	if (temp == "")
		age_value = 0;
	else age_value = std::stod(temp);
	
	double corrected_age_value = age_value + response_delay;
	initial_age = std::max(apparent_age, corrected_age_value);
}

