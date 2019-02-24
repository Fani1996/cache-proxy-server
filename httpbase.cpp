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
void httpBase::meta_parser(std::string metaline) {
    // split into vector of meta.
    // this->meta = split(meta, ' ');
    std::size_t pre = 0;
    std::size_t pos = metaline.find_first_of(' ');
    meta.push_back(metaline.substr(pre, pos-pre));

    pre = pos + 1;
    pos = metaline.find_first_of(' ', pre);
    meta.push_back(metaline.substr(pre, pos-pre));

    if(pos == std::string::npos)
        return;

    pre = pos + 1;
    pos = metaline.find_first_of(' ', pre);
    meta.push_back(metaline.substr(pre, pos-pre));

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
    std::string headerline;
    std::string metaline;

    int flag_firstline=0;
    while(1){
        char buffer[2];
        memset(&buffer, 0, sizeof(buffer));
        int actual_byte = sk.recv_msg(buffer, 1, 0);
        if(actual_byte == 0){
            std::cerr<<"connect closed"<<std::endl;
            //throw 
        }
        content.push_back(buffer[0]);
        header.push_back(buffer[0]);

        if(!strncmp(buffer, "\r", 1)){
            char check[2];
            memset(&check, 0, sizeof(check));
            actual_byte = sk.recv_msg(check, 1, 0);
            if(actual_byte==0){
                std::cerr<<"connect closed"<<std::endl;
            //throw 
            }
            content.push_back(check[0]);
            header.push_back(check[0]);

            if(!strncmp(check,"\n",1)){
                if(flag_firstline == 0){
                    flag_firstline = 1;
                    meta_parser(metaline);
                }
                else{
                    header_parser(headerline);
                    headerline.clear();

                    std::vector<char> check_end(2,0);
                    // memset(&check_end,0,sizeof(check_end));
                    actual_byte = sk.recv_msg(&check_end.data()[0], 2, 0);
                    if(actual_byte==0){
                        std::cerr<<"connect closed"<<std::endl;
                        //throw 
                    }
                    content.insert(content.end(), check_end.begin(), check_end.end());
                    header.insert(header.end(), check_end.begin(), check_end.end());
                    
                    std::string end_string(check_end.begin(), check_end.end());
                    if(!strncmp(end_string.c_str(),"\r\n",2)){
                        break;
                    }
                    else{
                        headerline.append(end_string);
                    }
                }
            }
            else{   
                if(flag_firstline==0)
                    metaline.push_back(check[0]);
                else
                    headerline.push_back(check[0]);
            }
        }
        else{
            if(flag_firstline==0){
                metaline.push_back(buffer[0]);
            }
            else{
                headerline.push_back(buffer[0]);
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

        std::vector<char> buffer(127, 0);
        if(sk.recv_msg(&buffer.data()[0], 127, 0) == 0){
            break;
        }
        payload.insert(payload.end(), buffer.begin(), buffer.end());
        content.insert(content.end(), buffer.begin(), buffer.end());
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
        std::vector<char> buffer(2, 0);
        sk.recv_msg(&buffer.data()[0], 1, 0);

        data.push_back(buffer[0]);
        payload.push_back(buffer[0]);
        content.push_back(buffer[0]);

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
                std::vector<char> contentbuf(length+2, 0);
                sk.recv_msg(&contentbuf.data()[0], length+2, MSG_WAITALL);

                payload.insert(payload.end(), contentbuf.begin(), contentbuf.end());
                content.insert(content.end(), contentbuf.begin(), contentbuf.end());

                std::cout<<"=== chuck received. ==="<<std::endl;
                std::cout<<"length: "<<length<<std::endl;
                std::cout<<"content: "<<std::string(content.begin(), content.end())<<std::endl;

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
        std::vector<char> contentbuf(length, 0);
        sk.recv_msg(&contentbuf.data()[0], length, MSG_WAITALL);

        payload.insert(payload.end(), contentbuf.begin(), contentbuf.end());
        content.insert(content.end(), contentbuf.begin(), contentbuf.end());

        std::cout<<"=== content length received. ==="<<std::endl;
        std::cout<<"length: "<<length<<std::endl;
        std::cout<<"content: "<<std::string(content.begin(), content.end())<<std::endl;
    }
}

std::string httpBase::get_content(){
    return std::string(content.begin(), content.end());
}

std::string httpBase::get_header_kv(std::string key){
    if(headerpair.find(key) != headerpair.end())
        return headerpair[key];
    return "";
}

// set the value of header key in the map, but not update the data in vector yet.
void httpBase::set_header_kv(std::string key, std::string value){
    headerpair[key] = value;
}

// void httpBase::update_header(std::string line){
//     if(std::size_t pos = header.find("\r\n\r\n") != std::string::npos){
//         std::string newline("\r\n" + line + "\r\n\r\n");
//         header.replace(pos, 4, newline);
//     }
//     else if(std::size_t pos = header.find("\n\n") != std::string::npos){
//         std::string newline("\n" + line + "\n\n");
//         header.replace(pos, 2, newline);
//     }
// }

// generate header based on meta and header vector.
std::vector<char> httpBase::generate_header(){
    std::vector<char> newheader;
    // write meta data.
    for(auto m:meta){
        std::copy(m.begin(), m.end(), std::back_inserter(newheader));
        newheader.push_back(' ');
    }

    // write return to new line.
    std::string endline("\r\n"); 
    std::copy(endline.begin(), endline.end(), std::back_inserter(newheader));

    // write header data.    
    // newheader.insert(newheader.end(), header.begin(), header.end());
    for(auto h:headerpair){
        std::copy(h.first.begin(), h.first.end(), std::back_inserter(newheader));
        newheader.push_back(':');
        std::copy(h.second.begin(), h.second.end(), std::back_inserter(newheader));
        std::copy(endline.begin(), endline.end(), std::back_inserter(newheader));
    }
    std::copy(endline.begin(), endline.end(), std::back_inserter(newheader));

    return header = newheader;
}

// refresh the entire content data.
void httpBase::refresh(){
    content = header;
    content.insert(content.end(), payload.begin(), payload.end());
}

std::string httpBase::get_cache_control(std::string key){
    if(cache_control.find(key) != cache_control.end())
        return cache_control[key];
    return "";
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
    // std::cout<<"send: "<<content<<std::endl;
    // std::cout<<"end send"<<std::endl;
    
    // char * buffer = new char [content.length()+1];
    // std::strcpy (buffer, content.c_str());

    sk.send_msg(&content.data()[0], content.size()+1);
    // delete[] buffer;
}

void httpBase::cache_control_parser(){
  if(headerpair.find("Cache-Control") != headerpair.end()){
    std::string whole_cache_control = headerpair["Cache-Control"];
    
    size_t pos = whole_cache_control.find(",");
    while(pos != std::string::npos){
        std::string single_control = whole_cache_control.substr(0, pos);
        
        size_t position = single_control.find('=');
        if(position != std::string::npos){
            cache_control[single_control.substr(0, position)] = single_control.substr(pos+1);
        }
        else{
            throw std::invalid_argument("invalid cache control format.");
        }

        whole_cache_control = whole_cache_control.substr(pos+1);
        pos = whole_cache_control.find(",");
    }    
  }
}

bool httpBase::can_cache(){
    if(cache_control.find("no-cache") != cache_control.end())
        return false;

    if(cache_control.find("no-store") != cache_control.end())
        return false;

    if(cache_control.find("private") != cache_control.end())
        return false;

    if(cache_control.find("must_revalidate") != cache_control.end())
        return false;

    return true;
}
