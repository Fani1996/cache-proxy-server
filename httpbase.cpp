#include "httpbase.h"
#include <iostream>
#include <sstream>
#include <cassert>

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

void httpBase::recv_header(HttpSocket &sk){
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
            if(!strcmp(check,"\n",1)){
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
                    if(!strcmp(check_end,"\r\n",2)){
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
                    meta.bush_back(check[0]);
                else
                    header.bush_back(check[0]);
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
    
}
void httpBase::recv_http1-0(HttpSocket & sk){
   // std::string content;
    while(1){
        char buffer[127];
        memset(&buffer,0,sizeof(buffer));
        if(sk.recv_msg(buffer, 127, 0) == 0){
            break;
        }
        content.append(buffer);
    }
    std::size_t pos = content.find_first_of('\n');
    std::string meta = content.substr(0, pos);
    meta_parser(meta);

    std::size_t pre = pos + 1;
    pos = content.find_first_of('\n', pre);
    while(pos != std::string::npos){
        std::string line = content.substr(pre, pos-pre);
        if(pos - pre == 1)
            break;

        header_parser(line);
    }
    payload = content.substr(pos+1);
}

void httpBase::recv_http1-1(HttpSocket & sk){
    if(how_to_read() == "chunk"){
        recv_chunked_encoding(sock);
    }
    else if(how_to_read() == "length"){
        recv_content_length(sock);
    }
    else{
        std::cerr<<"Bad Header!"<<std::endl;
        throw badHeader();
    }
}
