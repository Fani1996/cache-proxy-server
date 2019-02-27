#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <sstream>

#include <string>

#include <cstdlib>
#include <unistd.h>

class Log {
private:
    std::string path;
    std::string filename;

    std::string get_working_path(){
        char temp[512];
        return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
    }

public:
    Log() {
        path = get_working_path();
        filename = "log.txt";

        std::string target = "mkdir " + path + filename;
        system(target.c_str());
    }
    Log(std::string path, std::string filename) : path(path), filename(filename) {
        std::string target = "mkdir " + path + filename;
        system(target.c_str());
    }

    void output(std::string output);
};