#include "httpbase.h"
#include <iostream>
#include <sstream>
#include <string>

#include <cassert>

#include <stdexcept>      // std::length_error
#include <exception>


std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

// get ‘/abc/page1.php?s1=1&s2=xyz’ http/1.1
void httpBase::meta_parser(std::string meta) {
    // split into vector of meta.
    this->meta = split(meta, ' ');

    // std::size_t pre = 0;
    // std::size_t found = meta.find_first_of(' ');

    // while (found != std::string::npos) {
    //     this->meta.push_back(meta.substr(pre, found-pre));
    //     pre = found + 1;
    //     found = meta.find_first_of(' ', pre);
    // }
}

void httpBase::header_parser(std::string line) {
    std::vector<std::string> splitted = split(line, ':');
    headerpair[splitted[0]] = splitted[1];
}

//return value:1 chunk -1 length 0 none
int httpBase::recv_header(HttpSocket &sk){
    std::string header;
    std::string meta;
    int flag_firstline=0;
    while(1){
        char buffer[2];
        memset(&buffer,0,sizeof(buffer));
        int actual_byte=sk.recv_msg(buffer,1,0);
        if(actual_byte==0){
            std::cerr<<"connect closed"<<std::endl;
            //throw 
        }
        if(!strncmp(buffer,"\r",1)){
            char check[2];
            memset(&check,0,sizeof(check));
            int actual_byte=sk.recv_msg(check,1,0);
            if(actual_byte==0){
                std::cerr<<"connect closed"<<std::endl;
            //throw 
            }
            if(!strncmp(check,"\n",1)){
                if(flag_firstline==0){
                    flag_firstline=1;
                    meta_parser(meta);
                }
                else{
                    header_parser(header);
                    header.clear();
                    char check_end[3];
                    memset(&check_end,0,sizeof(check_end));
                    int actual_byte=sk.recv_msg(check_end,2,0);
                    if(actual_byte==0){
                        std::cerr<<"connect closed"<<std::endl;
                        //throw 
                    }
                    if(!strncmp(check_end,"\r\n",2)){
                        break;
                    }
                    else{
                        header.append(buffer);
                    }
                }
            }
            else
            {   
                if(flag_firstline==0)
                    meta.push_back(check[0]);
                else
                    header.push_back(check[0]);
            }
            
        }
        else{
            if(flag_firstline==0){
                meta.push_back(buffer[0]);
            }
            else{
                header.push_back(buffer[0]);
            }
        }

    }
    if(headerpair.find("Transfer-Encoding") != headerpair.end())
        return 1;
    else if(headerpair.find("Content-Length") != headerpair.end())
        return -1;
    else
        return 0;
    
}
void httpBase::recv_http_1_0(HttpSocket & sk){
    while(1){
        char buffer[127];
        memset(&buffer,0,sizeof(buffer));
        if(sk.recv_msg(buffer, 127, 0) == 0){
            break;
        }
        payload.append(buffer);
    }
}

void httpBase::recv_http_1_1(HttpSocket & sk, int type){
    if(type == 1){
        recv_chunk(sk);
    }
    else if(type == 0){
        recv_length(sk);
    }
    else{
        std::cerr<<"Invalid Header!"<<std::endl;
        //throw badHeader();
    }
}

void httpBase::recv_chunk(HttpSocket& sk) {
    std::string data;
    while(1){
        char buffer[2] = {0};
        sk.recv_msg(buffer, 1, 0);

        data.push_back(buffer[0]);
        payload.push_back(buffer[0]);

        unsigned long length = 0;

        // check for length.
        if(buffer[0] == '\n'){ // finish reciving.
            std::size_t pos = data.find_first_of(" \r");
            std::string lenstr = data.substr(0, pos);
            length = strtoul(lenstr.c_str(), NULL, 16);

            if(length == 0){ // finish reading.
                return;
            }
            else{ // storing data into payload.
                char* contentbuf = new char[length+3];
                memset(reinterpret_cast<void*>(contentbuf), 0, length+3);

                sk.recv_msg(contentbuf, length+2, MSG_WAITALL);
                payload.append(contentbuf, length+2);

                data = "";
            }
        }
    }
}

void httpBase::recv_length(HttpSocket& sk) {
    int length = -1;
    length = std::stoi(headerpair["Content-Length"]);

    if(length == -1){
        throw std::invalid_argument("invalid length");
    }
    else if(length > 0){
        char* contentbuf = new char[length+1];
        memset(reinterpret_cast<void*>(contentbuf), 0, length+1);

        sk.recv_msg(contentbuf, length, MSG_WAITALL);
        payload.append(contentbuf, length);
    }
}

