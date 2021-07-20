#ifndef __REDIS__H__
#define __REDIS__H__
#include <cpp_redis/cpp_redis>
#include<iostream>
#include "inc/TdConf.h"
#include <unistd.h>
class TdRedis
{
private:
    
    cpp_redis::subscriber sub;
    static void clientConnectCallback(const std::string& host, std::size_t port, cpp_redis::client::connect_state status);
    static void subscriberConnectCallback(const std::string& host, std::size_t port, cpp_redis::subscriber::connect_state status);
    /* data */
public:
    cpp_redis::client client;
    TdRedis(){};
    ~TdRedis(){};
    int clientConnect();
    int subcrsiberConnect();
};


#endif 


