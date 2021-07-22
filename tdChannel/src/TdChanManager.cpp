#include "TdChanManager.h"
TdChanManager * TdChanManager::mTdChanManager ;


TdChanManager* TdChanManager::getInstance(){
    if(mTdChanManager==nullptr)
    {
        mTdChanManager = new TdChanManager();
    }
    return mTdChanManager ;
}

int TdChanManager::createChannel(std::string channle,std::string dest,int destPort,int basePort)
{
    int status = -1 ;
    pthread_mutex_lock(&chanMapMutex);
    if(chanMap[channle])
    {
        pthread_mutex_unlock(&chanMapMutex);
        spdlog::error("通道({}) 已存在",channle);
        return status ;
    } 
    pthread_mutex_unlock(&chanMapMutex);
  
    TdRtp *gRtp = new TdRtp();
    status = gRtp->setUp(dest.c_str(),destPort,basePort);
    if(status<0)
    {
        delete gRtp ;
        spdlog::error("创建通道失败 原因:{}",status);
        return status ;
    }
    gRtp->start(1);
    pthread_mutex_lock(&chanMapMutex);
    chanMap[channle] = gRtp;
    pthread_mutex_unlock(&chanMapMutex);
    return 0 ;
}
int TdChanManager::delChannel(std::string channle)
{
    pthread_mutex_lock(&delChanMutex);
    delChanList.push_front(channle);
    pthread_mutex_unlock(&delChanMutex);
    return 0 ;
}
void TdChanManager::delChanWorkLoop(TdChanManager* chanMannager)
{
    chanMannager->delChanWorkStatus = true;
    while(chanMannager->delChanWorkStatus)
    {
        std::string chanId="";
        pthread_mutex_lock(&chanMannager->delChanMutex);
        if(!chanMannager->delChanList.empty()){
            chanId=chanMannager->delChanList.front();
            chanMannager->delChanList.pop_front();
        }
        pthread_mutex_unlock(&chanMannager->delChanMutex);
        
        if(!chanId.empty())
        {
            TdRtp * rtpPtr = nullptr ;
            pthread_mutex_lock(&chanMannager->chanMapMutex);
            rtpPtr = chanMannager->chanMap[chanId];
            pthread_mutex_unlock(&chanMannager->chanMapMutex);
            if(rtpPtr!=nullptr)
            {
                rtpPtr->stop();
                delete rtpPtr ;
                rtpPtr = nullptr ;
                pthread_mutex_lock(&chanMannager->chanMapMutex);
                chanMannager->chanMap.erase(chanId);
                pthread_mutex_unlock(&chanMannager->chanMapMutex);
            }
        }
        
        usleep(3000);
    }
}

