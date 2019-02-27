#include "log.h"
#include "httprequest.h"

#include <chrono>
#include <ctime>




void Log::output(std::string output){

    std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
    lck.lock();

    std::ofstream outfile;

    outfile.open(filepath, std::ios_base::app);
    outfile << output;

    outfile.close();

    lck.unlock();
}

void Log::timestamp(){
    std::time_t current = time(nullptr);

    std::stringstream timestr;
    timestr << "- At Time: " << std::ctime(&current);

    output(timestr.str());
}

void Log::log_request(HttpRequest& this_request){
    std::stringstream reqstr;

    reqstr << "- From: " << this_request.get_host() << ", with Port: " << this_request.get_port() << " -\n";
    reqstr << "- Method: " << this_request.get_method() << ", Protocol: " << this_request.get_version() << " -\n";

    output(reqstr.str());
}
