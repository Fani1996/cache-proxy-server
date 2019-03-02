#include "log.h"
#include "httprequest.h"

#include <chrono>
#include <ctime>




void Log::output(std::string output){
    std::ofstream outfile;

    outfile.open(filepath, std::ios_base::app);
    outfile << output;

    outfile.close();
}


void Log::output(std::string id, std::string output){
// void Log::output(int id, std::string output){

    std::ofstream outfile;

    outfile.open(filepath, std::ios_base::app);
    outfile << "[ " << id << " ]: " << output;

    outfile.close();
}

void Log::timestamp(std::string id){
    std::time_t current = time(nullptr);

    std::stringstream timestr;
    timestr << "@Time: " << std::ctime(&current);
    std::cout<<std::ctime(&current)<<std::endl;
    output(id, timestr.str());
}

void Log::log_request(std::string id, HttpRequest& this_request){
    std::stringstream reqstr;

    reqstr << "- From: " << this_request.get_host() << ", with Port: " << this_request.get_port() << " -\n";
    reqstr << "- Method: " << this_request.get_method() << ", Protocol: " << this_request.get_version() << " -\n";

    output(id, reqstr.str());
}

