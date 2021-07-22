#ifndef __TD_CONF_H_
#define __TD_CONF_H_ 

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "inc/uuid4.h"
#include "inc/inifile.h"
using namespace inifile;

//  new TdDevice("34020000002000000001", "192.168.11.195", 15060, 
//             "31011500991320000046", "admin", "admin123", 5060, "TDWL")
//         );
typedef struct {
    std::string mDevideId = "31011500991320000046";
    std::string mServerSipId = "34020000002000000001";
    std::string mServerSipDomain = "3402000000";
    std::string mSipServerIp = "192.168.11.195";
    int mSipServerPort = 15060;
    std::string mSipUserName ="admin";
    std::string mSipPassWd ="admin123";
    int mExpires =3600;
    int mBasePort = 5060;
    std::string mManufacture = "TDWL";
}GB28121Ctx ;
class TdConf
{
    private:
    IniFile mIni;
    GB28121Ctx mGB28121Ctx;
    static TdConf* mConf ;
    std::string confFile = "config.ini" ;
    public:
        static TdConf* getInstance();
        int saveConfig();
        int readConfig();
};

#endif