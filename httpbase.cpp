#include "httpbase.h"
#include <iostream>
#include <sstream>
#include <cassert>


void httpBase::meta_parser(std::string meta) {
    // split into vector of meta.
    std::size_t found = str.find_first_of("aeiou");
    while (found!=std::string::npos)
    {
        str[found]='*';
        found=str.find_first_of("aeiou",found+1);
    }
}

void httpBase::header_parser() {

}

void httpBase::payload_parser() {
    
}

void httpBase::parser() {
    // parse the line of meta.
    auto pos = this->content.find_first_of('\n');
    std::string meta = this->content.substr(0, pos);

}
