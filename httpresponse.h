#ifndef _HTTPRESPONSE__H
#define _HTTPRESPONSE__H

#include <unordered_map>
#include <string>
#include <vector>

#include "httpsocket.h"

clasee HttpResponse:public httpBase{
 private:
  std::string content;
 public:
    std::string get_version(){};
}
#endif
