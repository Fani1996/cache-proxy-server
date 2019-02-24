#ifndef _HTTPRESPONSE__H
#define _HTTPRESPONSE__H

#include <unordered_map>
#include <string>
#include <vector>
#include <time.h>
#include "httpsocket.h"
#include "httpbase.h"

class HttpResponse : public httpBase {
private:
  //  t_time response_time;
  // t_time initial_age;
public:
    // constructor
    HttpResponse() {}

    std::string get_code();

    void receive(HttpSocket &sk);
};

#endif
