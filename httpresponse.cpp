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
		send_400_bad_request(sk);
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
			send_400_bad_request(sk);
			//throw
			throw std::invalid_argument("invalid version.");
		}
	}
	
    //calculate response_time
    time_t rawtime=time(NULL);
    struct tm * ptm;
    //    memset(ptm, 0, sizeof(struct tm));
    //time ( &rawtime );
    ptm = gmtime ( &rawtime );
    response_time = mktime(ptm);
    //response_time=time(NULL);
}

std::string HttpResponse::get_code(){
	return meta[1];
}
double HttpResponse::get_fresh_lifetime(){
	double fresh_lifetime;
	std::string temp = get_cache_control("s-maxage");
	if(temp == ""){
		temp = get_cache_control("max-age");
		std::cout<<"max age:  "<<temp<<std::endl;
	}
	else{
	  //s-maxage
		fresh_lifetime = std::stod(temp);
		std::cout<<"===fresh lifetime is:: "<<fresh_lifetime<<"==="<<std::endl;
		return fresh_lifetime;
	}

	if(temp==""){
		temp=get_header_kv("Expires");
	}
	else{
	  //max-age
		fresh_lifetime=std::stod(temp);
		std::cout<<"===fresh lifetime is:: "<<fresh_lifetime<<"==="<<std::endl;
		return fresh_lifetime;
	}

	//Heuristic Freshness
	if(temp == ""){
		struct tm last_modified_tm;
		memset(&last_modified_tm, 0, sizeof(struct tm));
		std::cout<<"last modified"<<get_header_kv("Last-Modified")<<std::endl;
		strptime(get_header_kv("Last-Modified").c_str(), " %a, %d %b %Y %H:%M:%S %Z", &last_modified_tm);
		
		time_t last_modified = mktime(&last_modified_tm);
		fresh_lifetime = difftime(time(NULL),last_modified)*0.1;
		std::cout<<"===fresh lifetime is:: "<<fresh_lifetime<<"==="<<std::endl;
	}
	//expires
	else{
		fresh_lifetime = std::stod(temp)-std::stod(get_header_kv("Date"));
		std::cout<<"===fresh lifetime is:: "<<fresh_lifetime<<"==="<<std::endl;
	}

	return fresh_lifetime;
}
double HttpResponse::get_current_age(){
        time_t rawtime=time(NULL);
        struct tm * ptm;
        ptm = gmtime (&rawtime);
	 time_t nowtime = mktime(ptm);
        double resident_time = difftime(nowtime,response_time);
	std::cout<<"===resident_time is ::"<<resident_time<<"==="<<std::endl;
	return initial_age+resident_time;
}

bool HttpResponse::is_fresh(){
  double fresh_lifetime=get_fresh_lifetime();
    double current_age = get_current_age();
    std::cout<<"===current age is:: "<<current_age<<"==="<<std::endl;
    return fresh_lifetime > current_age;
}

void HttpResponse::calculate_initial_age(time_t request_time){
	struct tm date_value_tm;
	memset(&date_value_tm, 0, sizeof(struct tm));
	std::cout<<"Date:"<<get_header_kv("Date")<<std::endl;
	strptime(get_header_kv("Date").c_str(), " %a, %d %b %Y %H:%M:%S %Z", &date_value_tm);
	
	time_t date_value = mktime(&date_value_tm);
	double apparent_age = difftime(response_time,date_value);
	std::cout<<"apparent_age:: "<<apparent_age<<std::endl;
	if(apparent_age < 0) apparent_age = 0;
	
	double response_delay = difftime(response_time, request_time);
	std::cout<<"response_delay"<<response_delay<<std::endl;
	double age_value;
	std::string temp = get_header_kv("Age");
	if (temp == "")
		age_value = 0;
	else age_value = std::stod(temp);
	std::cout<<"age is:"<<age_value<<std::endl;
	double corrected_age_value = age_value + response_delay;
	initial_age = std::max(apparent_age, corrected_age_value);
}
