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
    std::string filepath;

    std::string get_working_path(){
        char temp[512];
        return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
    }

public:
    Log() {
        filepath = get_working_path() + "/log.txt";
    }
    Log(std::string path, std::string filename) {
        filepath = path + filename;
    }

    void output(std::string output);
};