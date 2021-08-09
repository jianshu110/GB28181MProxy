#ifndef __CHANNEL_MANAGER_H__
#define __CHANNEL_MANAGER_H__
#include <iostream>
#include <list>
#include <map>
#include "inc/TdRtp.h"
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<sys/msg.h>
#include"inc/json11.hpp"
typedef struct {
    int pid;
    FifoMsgSession mFifos;
    std::string chanName;
    int height = 540;
    int width = 960;
    int Fps = 25;
}Channel;

class TdChanManager
{
private:
    TdChanManager(/* args */){
        pthread_mutex_init(&delChanMutex,NULL);
        pthread_mutex_init(&chanMapMutex,NULL);
        std::thread t(delChanWorkLoop,this);
        t.detach();
    };
    ~TdChanManager(){};
    static TdChanManager * mTdChanManager;
    std::list<std::string> delChanList ;
    std::map<std::string ,Channel*> chanMap;
    static void delChanWorkLoop(TdChanManager* chanMannager);
    pthread_mutex_t delChanMutex;
    pthread_mutex_t chanMapMutex;
    bool delChanWorkStatus =false ;
    
    /* data */
public:
    static TdChanManager* getInstance();
    int monitor();
    int createChannel(std::string channle,std::string dest,int destPort,int basePort);
    int delChannel(std::string channle);
    std::string channelsInfo2Json();
};

#endif

