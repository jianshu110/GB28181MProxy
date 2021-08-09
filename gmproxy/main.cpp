#include "inc/TdDevice.h"
#include "inc/TdHttpServer.h"
#include "inc/TdH264.h"
#include "inc/TdConf.h"
#include "inc/Tdlog.h"
#include "inc/System.h"

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
    TdConf::getInstance();
    TdHttpServer * httpServer  = new TdHttpServer();
    httpServer->init();
    auto device = shared_ptr<TdDevice>(
        new TdDevice("34020000002000000001", "192.168.16.195", 15060, 
            "31011500991320000046", "admin", "admin123", 5060, "TDWL")
        );
    device->init();
    return 0;
}