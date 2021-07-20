#include "TdRedis.h"


int TdRedis::clientConnect()
{
    client.connect("0.0.0.0",6379,clientConnectCallback,3000,5,6000);
}

int TdRedis::subcrsiberConnect(){
    sub.connect("0.0.0.0",6379,subscriberConnectCallback,3000,5,6000);
    while (!sub.is_connected())
    {
        printf("正在连接redis.....\r\n");
        usleep(100000);
    }
    printf("redis连接成功\r\n");
    std::string chan = TdConf::getInstance()->getUuid();
    std::cout << chan << std::endl;
    sub.subscribe(chan, [](const std::string& chan, const std::string& msg) {
    std::cout << "MESSAGE " << chan << ": " << msg << std::endl;
    });
    sub.commit();
}

void TdRedis::subscriberConnectCallback(const std::string& host, std::size_t port, cpp_redis::subscriber::connect_state status){
    printf("subscriber: %s %d\r\n",host.c_str(),status);
}
void TdRedis::clientConnectCallback(const std::string& host, std::size_t port, cpp_redis::client::connect_state status)
{
    printf("%s %d\r\n",host.c_str(),status);
}