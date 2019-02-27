#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

class Log {
private:
    std::string path;
    std::string filename;

    std::string get_working_path(){
        std::size_t MAXPATHLEN = 512;
        char temp[MAXPATHLEN];
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

    void output(std::string output){
        std::cin >> output;
        std::ofstream out(path + filename);
        out << output;

        out.close();
    }
};