#include "httpbase.h"
#include <iostream>
#include <sstream>
#include <string>

#include <cassert>

#include <stdexcept>      // std::length_error
#include <exception>


std::vector<std::string> httpBase::split (const std::string &s, char delim) {
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

    for(auto metaa:this->meta){
        std::cout<<"meta received: "<<metaa<<std::endl;
    }
}

void httpBase::header_parser(std::string line) {
    std::size_t pos = line.find_first_of(':');
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos+1);

    headerpair[key] = value;

    std::cout<<"header parsed: key: "<<key<<", value: "<<value<<std::endl;
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
                        header.append(check_end);
                        content.append(check_end);
                    }
                }
            }
            else{   
                if(flag_firstline==0)
                    meta.push_back(check[0]);
                else
                    header.push_back(check[0]);
                content.push_back(check[0]);
            }
            
        }
        else{
            if(flag_firstline==0){
                meta.push_back(buffer[0]);
            }
            else{
                header.push_back(buffer[0]);
            }
            content.push_back(buffer[0]);
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
    else if(type == -1){
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

                std::cout<<"=== chuck received. ==="<<std::endl;
                std::cout<<"length: "<<length<<std::endl;
                std::cout<<"content: "<<std::string(contentbuf)<<std::endl;

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

        std::cout<<"=== content length received. ==="<<std::endl;
        std::cout<<"length: "<<length<<std::endl;
        std::cout<<"content: "<<std::string(contentbuf)<<std::endl;
    }
}

std::string httpBase::get_content(){
    return content;
}

void httpBase::send_400_bad_request(HttpSocket& sk){
    std::string error("HTTP/1.1 400 Bad Request\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n400 Bad Request");
    sk.send_msg(const_cast<char *>(error.c_str()), error.size());
}

void httpBase::send_502_bad_gateway(HttpSocket& sk){
    std::string error("HTTP/1.1 502 Bad Gateway\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n502 Bad Gateway");
    sk.send_msg(const_cast<char *>(error.c_str()), error.size());
}

void httpBase::send(HttpSocket sk){
    char * buffer = new char [content.length()+1];
    std::strcpy (buffer, content.c_str());
    sk.send_msg(buffer, sizeof(char) * content.length());
    delete[] buffer;
}
