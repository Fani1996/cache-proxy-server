#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>

#include <filesystem>


class Log {
private:
    std::string path;
    std::string filename;

public:
    Log() {
        path = std::filesystem::current_path();
        filename = "log.txt";
        system("mkdir " + path + filename);
    }
    Log(std::string path, std::string filename) : path(path), filename(filename) {
        system("mkdir " + path + filename);
    }

    void output(std::string output){
        std::cin >> output;
        std::ofstream out(path + filename);
        out << output;
        
        out.close();
    }
};