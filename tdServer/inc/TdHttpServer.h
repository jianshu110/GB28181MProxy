#ifndef __TD_HTTPSERVER_H_
#define __TD_HTTPSERVER_H_
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include "inc/json11.hpp"
#include"httplib.h"
#include <thread>
#include "inc/noticeCenter.h"
#include <fstream>

using namespace httplib;
using namespace toolkit;
#define PAGE "<html><head><title>GMProxy</title>"\
             "</head><body>GMProxy</body></html>"
class TdHttpServer
{
private:
    httplib::Server mHttpServer;
    static void httploop(TdHttpServer* httpSrv);
    std::thread httpTh ;
public:
    TdHttpServer(/* args */){};
    ~TdHttpServer(){};
    uint32_t init();
};

#endif