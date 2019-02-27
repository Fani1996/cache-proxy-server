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
   time_t response_time;
   double initial_age;
public:
    // constructor
    HttpResponse() {}

    std::string get_code();
    double get_fresh_lifetime();
    bool is_fresh();
    double get_current_age();
    void calculate_initial_age(time_t request_time);
    void receive(HttpSocket &sk);
    bool must_revalidate();
};

#endif
