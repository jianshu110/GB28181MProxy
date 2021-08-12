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
    Resolution_t mRes= Resolution_t("WQVGA") ;
    int Fps = 25;
    int MaxBitrate = 300000;
}ChanMediaParam_t;


typedef struct {
    int pid;
    FifoMsgSession mFifos;
    std::string chanName;
    ChanMediaParam_t mCMediaParam;
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
    int createChannel(std::string channle,std::string dest,int destPort,int basePort,ChanMediaParam_t param);
    int delChannel(std::string channle);
    std::string channelsInfo2Json();
    int mediaPreset(std::string chan,ChanMediaParam_t param);
};

#endif

