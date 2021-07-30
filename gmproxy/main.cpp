#include "inc/TdDevice.h"
#include "inc/TdHttpServer.h"
#include "inc/TdH264.h"
#include "inc/TdConf.h"
#include "inc/Tdlog.h"
#include "inc/System.h"

TdH264 gH264;

uint8_t iDate[111];
uint32_t iAa ;

uint8_t *oDate;
uint32_t oAa ;
int pid ;
//std::string dest,int destPort,int basePort
  void f1(){
        //std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        //printf("f1\r\n");
        std::string channel = "345689fds";
        int destportId = 31002;
        int baseportId = 30002;
        
        while(1)
        {
            destportId += 2;
            baseportId += 2;
            channel= std::to_string(baseportId);
            // TdChanManager::getInstance()->createChannel(channel,"192.168.11.126",destportId,baseportId);
            // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            // TdChanManager::getInstance()->delChannel(channel);
            // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            float memory_usage = GetMemoryUsage(pid);
            std::cout << "memory usage: " << memory_usage << "MB" << std::endl;
        }
    }

int main(){

    char logo[] = "\n\
  ____ __  __ ____\n\
 / ___|  \\/  |  _ \\ _ __ _____  ___   _\n\
| |  _| |\\/| | |_) | '__/ _ \\ \\/ / | | |\n\
| |_| | |  | |  __/| | | (_) >  <| |_| |\n\
 \\____|_|  |_|_|   |_|  \\___/_/\\_\\__,  |\n\
                                  |___/\n\n";
    printf("%s",logo);

char explain[] = "* Copyright (c) 2021 The GMProxy project authors. All Rights Reserved.\n\
*\n\
* This file is part of GMProxy(https://gitee.com/jianshu20121/GMProxy.git).\n\
*\n\
* Use of this source code is governed by GPL-2.0 license that can be found in the\n\
* LICENSE file in the root of the source tree. All contributing project authors\n\
* may be found in the AUTHORS file in the root of the source tree. \n\n";
 printf("%s",explain);
    //spdlog::set_level(spdlog::level::debug);
    SET_LOG_LEVEL(LOG_LEVEL_INFO);
    TdConf::getInstance();
  
    // LOG_TRACE("你的");
    // LOG_INFO("你的");
    // LOG_WARNING("你的");
    // LOG_ERROR("你的");

  
    pid = GetCurrentPid();
    printf("pid: %d\r\n",pid);
    TdHttpServer * httpServer  = new TdHttpServer();
    httpServer->init();
    auto device = shared_ptr<TdDevice>(
        new TdDevice("34020000002000000001", "192.168.16.195", 15060, 
            "31011500991320000046", "admin", "admin123", 5060, "TDWL")
        );
    std::thread t01(f1);
    device->start();
    return 0;
}