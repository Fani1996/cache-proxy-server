#include "log.h"


void Log::output(std::string output){
    std::cin >> output;
    std::ofstream out(path + filename);
    out << output;

    out.close();
}