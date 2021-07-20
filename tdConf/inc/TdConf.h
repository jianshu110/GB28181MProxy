#ifndef __TD_CONF_H_
#define __TD_CONF_H_ 

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "inc/uuid4.h"
class TdConf
{
    private:
    std::ofstream *confFb ;
    std::string mUuid = "" ;
    static TdConf* mConf ;
    public:
        static TdConf* getInstance();
        std::string getUuid();
        void setUuid(std::string Uuid);
};

#endif