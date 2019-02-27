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

void Log::timestamp(){
    auto current = std::chrono::system_clock::now();

    std::time_t end_time = std::chrono::system_clock::to_time_t(current);
    std::ctime(&end_time);

    std::stringstream timestr;
    timestr << "- At Time: " << end_time << " -\n";

    output(timestr.str());
}

void Log::log_request(HttpRequest& this_request){
    std::stringstream reqstr;

    reqstr << "- From: " << this_request.get_host() << ", with Port: " << this_request.get_port() << " -\n";
    reqstr << "- Method: " << this_request.get_method() << ", Protocol: " << this_request.get_version() << " -\n";

    output(reqstr.str());
}
