#ifndef _HTTPRESPONSE__H
#define _HTTPRESPONSE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"
#include "httpbase.h"

class HttpResponse : public httpBase{
 private:
  std::string content;
 public:
    void receive(HttpSocket &sk);
};
#endif
