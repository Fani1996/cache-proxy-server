#include "httpbase.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <cassert>
#include<algorithm>
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
void httpBase::meta_parser() {
    // split into vector of meta.
    // this->meta = split(meta, ' ');
    std::size_t pre = 0;
    //    char *temp=std::strstr(content.data(),"\r\n");
    /*    
	  char *headertemp=content.data();
    std::cout<<"META_PARSER_SIZE:"<<temp-headertemp+1<<std::endl;
    char *metatemp=new char[temp-headertemp+1];
    strncpy(metatemp, content.data(),temp-headertemp);
        metatemp[temp-headertemp] = '\0';
    */
    
    //std::string metaline=metatemp;
    std::string metaline = std::string(content.data());
    std::size_t first_line = metaline.find("\r\n");

    metaline = metaline.substr(0, first_line);
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

   
    //    delete[] metatemp;
}
/*
void httpBase::header_parser(std::string line) {
    std::size_t pos = line.find_first_of(':');
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos+1);

    headerpair[key] = value;

    if(key == "Host"){
        std::string hoststr(value);
        hoststr.erase(std::remove(hoststr.begin(), hoststr.end(), ' '), hoststr.end());

        auto pos = meta[1].find(hoststr);
        if(pos != std::string::npos){
            meta[1] = meta[1].substr(pos + value.size() - 1);
            std::cout<<meta[1]<<std::endl;
        }
    }

    std::cout<<"header parsed: key: "<<key<<", value: "<<value<<std::endl;
}
*/
//return value:1 chunk -1 length 0 none
int httpBase::recv_header(HttpSocket &sk){
  //  char * contentbuf = new char[1024];
  //  std::vector<char> contentbuf;
  //std::size_t actual_byte = sk.recv_msg(&contentbuf.data()[0],1024,0);//std::cout<<cap<<" line 336\r\n";
  content.clear();
  content=std::vector<char>(33554432,0);
  std::size_t actual_byte = sk.recv_msg(content.data(),1024,0);//std::cout<<cap<<" line 336\r\n";

  //  content.insert(content.end(), contentbuf.begin(), contentbuf.end());
  //content.push_back('\0');
  if(actual_byte == 0){
    std::cerr<<"connect closed"<<std::endl;
    //throw
    throw std::exception();
  }
  content.resize(actual_byte);
  
  if(strstr(content.data(), "\r\n\r\n")==NULL){
    return 0;
    //throw
  }
  size_t pos = strstr(content.data(), "\r\n\r\n") - content.data();
  pos+=4;
  header_len = pos;
  //header.insert(header.end(), content.begin(), content.begin() + pos-1);
  //header.push_back('\0');
  //if(pos+1 < actual_byte){
    //payload.insert(payload.end(), content.begin() + pos, content.end());
  //payload.push_back('\0');
  //}
  //  std::cout<<"RECV_HEADER:\nCONTENT:\n=>"<<std::string(content.begin(), content.begin()+pos)<<std::endl;
  // std::cout<<"RECV_HEADER:\nHEADER:\n=>"<<std::string(header.begin(), header.end())<<std::endl;
  //std::cout<<"RECV_HEADER:\nPAYLOAD:\n=>"<<std::string(payload.begin(), payload.end())<<std::endl;

  meta_parser();
  cache_control_parser();

  
  char * headertemp=content.data();
  
	char * te=strstr(headertemp,"Transfer-Encoding");
	  char* cl=strstr(headertemp,"Content-Length");

	  //    delete[] contentbuf;

	if(te!=NULL)
	  return 1;
	else if(cl !=NULL)
	  return -1;
        else
	  return 0;
}

void httpBase::recv_http_1_0(HttpSocket & sk){
    while(1){
        std::vector<char> buffer(127, 0);
        try{
            if(sk.recv_msg(&buffer.data()[0], 127, 0) == 0){
                break;
            }
        }
        catch(...){
            throw std::exception();
        }
        payload.insert(payload.end(), buffer.begin(), buffer.end());
        content.insert(content.end(), buffer.begin(), buffer.end());
    }
}

void httpBase::recv_http_1_1(HttpSocket & sk, int type){
    if(type == 1){
        try{
	  std::cout<<"recv chunk"<<std::endl;
            recv_chunk(sk);
        }
        catch(...){
            throw std::exception();
        }
    }
    else if(type == -1){
        try{
            recv_length(sk);
        }
        catch(...){
            throw std::exception();
        }
    }
    else{
        std::cerr<<"Invalid Header!"<<std::endl;
        //throw badHeader();
        throw std::invalid_argument("invalid header");
    }
}
/*
void httpBase::recv_chunk(HttpSocket& sk) {
    std::string data;
    //    if()
    while(1){
        std::vector<char> buffer(2, 0);
        try{
            sk.recv_msg(&buffer.data()[0], 1, 0);
        }
        catch(...){
            buffer.clear();
            throw std::exception();
        }
        data.push_back(buffer[0]);
        payload.push_back(buffer[0]);
        content.push_back(buffer[0]);

        // check for length.
        unsigned long length = 0;
        if(buffer[0] == '\n'){ // finish reciving.
            std::size_t pos = data.find_first_of(" \r");
            std::string lenstr = data.substr(0, pos);
            length = strtoul(lenstr.c_str(), NULL, 16);

            if(length == 0){ // finish reading.
                return;
            }
            else{ // storing data into payload.
                std::vector<char> contentbuf(length+2, 0);
                try{
                    sk.recv_msg(&contentbuf.data()[0], length+2, MSG_WAITALL);
                }
                catch(...){
                    contentbuf.clear();
                    throw std::exception();
                }

                payload.insert(payload.end(), contentbuf.begin(), contentbuf.end());
                content.insert(content.end(), contentbuf.begin(), contentbuf.end());

                //std::cout<<"=== chuck received. ==="<<std::endl;
                //std::cout<<"length: "<<length<<std::endl;
                //std::cout<<"content: "<<std::string(content.begin(), content.end())<<std::endl;

                data = "";
            }
        }
    }
    }*/

void httpBase::recv_chunk(HttpSocket& sk) {
  if(strstr(content.data(),"0\r\n\r\n")!=NULL){
    std::cout<<"payload already in content"<<std::endl;
      return;
  }
  else{
    int curr_len = content.size();
    content.resize(33554432);
    int actual_byte=0;
    while(1){
      std::cout<<"i m chunked!!"<<std::endl;
      //        std::vector<char> contentbuf(2048, 0);
      std::cout<<"actual_byte::"<<actual_byte<<std::endl;
      curr_len+=actual_byte;
       if(strstr(content.data(),"0\r\n\r\n")!=NULL){
           std::cout<<"end of chunk!!!"<<std::endl;
           content.resize(curr_len);
	   std::cout<<"content received===> "<<std::endl;
	   std::cout<<content.data()<<std::endl;
           return;
        }
      try{
          actual_byte=sk.recv_msg(content.data()+curr_len, 1024, 0);
        }
        catch(...){
	  //            contentbuf.clear();
            throw std::exception();
        }
	//std::cout<<"content received: "<<content.data()<<std::endl;
	//	payload.insert(payload.end(), contentbuf.begin(), contentbuf.end());
	//        content.insert(content.end(), contentbuf.begin(), contentbuf.end());
    }	
  }
}

void httpBase::recv_length(HttpSocket& sk) {
    int length = -1;
    int curr_len=content.size();
    length = std::stoi(get_header_kv("Content-Length"));
    if(length == -1){
        throw std::invalid_argument("invalid length");
    }
    else if(length > 0){
      if((curr_len-header_len) == length)
	    return;
      //        std::vector<char> contentbuf(length-payload.size(), 0);
        content.resize(length+header_len);
        payload_len = curr_len - header_len;
	try{
	  sk.recv_msg(content.data()+curr_len, length - payload_len, MSG_WAITALL);
        }
        catch(...){
	  //            contentbuf.clear();
            throw std::exception();
        }
	//        payload.insert(payload.end(), contentbuf.begin(), contentbuf.end());
	//        content.insert(content.end(), contentbuf.begin(), contentbuf.end());

        //std::cout<<"=== content length received. ==="<<std::endl;
        //std::cout<<"length: "<<length<<std::endl;
        //std::cout<<"content: "<<std::string(content.begin(), content.end())<<std::endl;
    }
}

std::vector<char> httpBase::get_content(){
  return content;
}


std::string httpBase::get_header_kv(std::string key){
    char * result = content.data();
    const char * target = key.c_str();

    //    char * value;
    char *temp=std::strstr(result, target);
    if( temp!= NULL ){ // find key.
        char *temp2= std::strstr(temp, ":");
        if(temp2 != NULL){ // find :.
            char * pos = std::strstr(temp2, "\r\n");
            if(pos != NULL){ // find the end.
	      char * copyed =new char[pos-temp2];
		strncpy(copyed, temp2+1, pos-temp2-1);
		copyed[pos-temp2-1] = '\0';
    //    std::cout<< "GET_HEADER_KV: key: "<<key<<", parsed header: " << copyed << std::endl;
                std::string value = std::string(copyed);
                delete[] copyed;
                return value;
            }
        }
    }   
    else{
        return "";
    }
    // if(headerpair.find(key) != headerpair.end())
    //     return headerpair[key];
     return "";
}


/*
std::stirng httpBase::update_header(httpBase updated){
}
*/

// set the value of header key in the map, but not update the data in vector yet.
void httpBase::set_header_kv(std::vector<char> key, std::vector<char> value){
  std::vector<char> headerline=key;
  headerline.push_back(':');
  headerline.push_back(' ');
  headerline.insert(headerline.end(),value.begin(),value.end());
  headerline.push_back('\r');
  headerline.push_back('\n');
  content.insert(content.begin()+header_len-2,headerline.begin(),headerline.end());
  // headerpair[key] = value;
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

/*
// generate header based on meta and header vector.
std::vector<char> httpBase::generate_header(){
    std::vector<char> temp(content);
    std::vector<char> newheader;

    try{
        // write meta data.
        for(std::size_t i=0; i<meta.size()-1; i++){
            auto m = meta[i];
            std::copy(m.begin(), m.end(), std::back_inserter(newheader));
            newheader.push_back(' ');
        }
        auto m = meta[meta.size()-1];
        std::copy(m.begin(), m.end(), std::back_inserter(newheader));

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
    }
    catch(...){
        newheader.clear();
        return header = temp;
    }

    return header = newheader;
}

// refresh the entire content data.
void httpBase::refresh(){
    content = header;
    content.insert(content.end(), payload.begin(), payload.end());
}
*/
std::string httpBase::get_cache_control(std::string key){
    if(cache_control.find(key) != cache_control.end())
        return cache_control[key];
    return "";
}

void httpBase::send_400_bad_request(HttpSocket& sk){
    std::string error("HTTP/1.1 400 Bad Request\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n400 Bad Request");
    try{
        sk.send_msg(const_cast<char *>(error.c_str()), error.size());
    }
    catch(...){
        return;
    }
}

void httpBase::send_502_bad_gateway(HttpSocket& sk){
    std::string error("HTTP/1.1 502 Bad Gateway\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n502 Bad Gateway");
    try{
        sk.send_msg(const_cast<char *>(error.c_str()), error.size());
    }
    catch(...){
        return;
    }
}

void httpBase::send(HttpSocket sk){
    // std::cout<<"send: "<<content<<std::endl;
    // std::cout<<"end send"<<std::endl;
    
    // char * buffer = new char [content.length()+1];
    // std::strcpy (buffer, content.c_str());
    try{
        sk.send_msg(&content.data()[0], content.size());
    }
    catch(...){
        throw std::bad_exception();
    }
    // delete[] buffer;
}

void httpBase::cache_control_parser(){
    std::string cache_control_header = get_header_kv("Cache-Control");
  if(cache_control_header != ""){
    std::string whole_cache_control = cache_control_header;
    std::cout<<"cache_control!!!!!"<<whole_cache_control<<std::endl;
    whole_cache_control.erase(std::remove(whole_cache_control.begin(), whole_cache_control.end(), ' '), whole_cache_control.end());
    
    size_t pos = whole_cache_control.find(",");
    while(pos != std::string::npos){
        std::string single_control = whole_cache_control.substr(0, pos);
	std::cout<<"single_control====="<<single_control<<std::endl;
        size_t position = single_control.find('=');
        if(position != std::string::npos){
            cache_control[single_control.substr(0, position)] = single_control.substr(position+1);
	    std::cout<<single_control.substr(0, position)<<std::endl;
        }
        else{
            cache_control[single_control]=single_control;
        }
	
        whole_cache_control = whole_cache_control.substr(pos+1);
        pos = whole_cache_control.find(",");
    }
    std::string single_control = whole_cache_control;
    size_t position = single_control.find('=');
        if(position != std::string::npos){
            cache_control[single_control.substr(0, position)] = single_control.substr(position+1);
            std::cout<<single_control.substr(0, position)<<std::endl;
        }
        else{
          cache_control[single_control]=single_control;
        }

  }
}

bool httpBase::can_store(){
    if(cache_control.find("no-store") != cache_control.end())
        return false;

    //    if(cache_control.find("private") != cache_control.end())
    //  return false;

    return true;
}

bool httpBase::no_cache(){
  if(cache_control.find("no-cache") != cache_control.end())
        return true;
  return false;
}
bool httpBase::must_revalidate(){
  if(cache_control.find("must-revalidate") != cache_control.end())
        return true;
  return false;
}
