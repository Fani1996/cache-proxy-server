#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

#include "httprequest.h"



class Log {
private:
    std::string filepath;

    std::string get_working_path(){
        char temp[512];
        return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
    }

public:
    Log() {
        filepath = "/var/log/erss/proxy.log";
    }
    Log(std::string path, std::string filename) {
        filepath = path + filename;
    }

    void output(std::string output);
    void output(std::string id, std::string output);
    void timestamp(std::string id);
    void log_request(std::string id, HttpRequest& request);
    
};