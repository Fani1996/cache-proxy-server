#include "log.h"


void Log::output(std::string output){

    std::ofstream outfile;

    outfile.open(filepath, std::ios_base::app);
    outfile << output;

    outfile.close();
}