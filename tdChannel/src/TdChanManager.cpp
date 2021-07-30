#include "TdChanManager.h"
TdChanManager * TdChanManager::mTdChanManager ;


TdChanManager* TdChanManager::getInstance(){
    if(mTdChanManager==nullptr)
    {
        mTdChanManager = new TdChanManager();
        mTdChanManager->monitor();
    }
    return mTdChanManager ;
}

int TdChanManager::monitor()
{
    TdChanManager *mprj = this ;
    NoticeCenter::Instance().addListener(this,"setCodecParam",[mprj](std::string channel,int Rate,std::string Resolution,int MaxBitrate){
    TdChanManager *mTag = mprj ;
    TdRtp *rtp = mTag->chanMap[channel];
    if(rtp==nullptr)
        return  ;
    CodecParam lCodecpara;
    memset(&lCodecpara,0,sizeof(CodecParam));
    spdlog::info("修改视频参数为-> 帧率:{} 分辨率:{} 最大比特:{}",Rate,Resolution,MaxBitrate);
    if(!Resolution.compare("1080p"))
    {
        lCodecpara.height = 1080 ;
        lCodecpara.width = 1920;
    }
    else if(!Resolution.compare("720p"))
    {
        lCodecpara.height = 720 ;
        lCodecpara.width = 1280;
    }
    else if(!Resolution.compare("qHD"))
    {
        lCodecpara.height = 540 ;
        lCodecpara.width = 960;
    }
    else if(!Resolution.compare("nHD"))
    {
        lCodecpara.height = 360 ;
        lCodecpara.width =  640;
    }
    else if(!Resolution.compare("WQVGA"))
    {
        lCodecpara.height = 272 ;
        lCodecpara.width =  480;
    }
    else if(!Resolution.compare("FWQVGA"))
    {
        lCodecpara.height = 240 ;
        lCodecpara.width =  432;
    }
    lCodecpara.rate = Rate;
    lCodecpara.maxBitrate = MaxBitrate;
    rtp->setParam(lCodecpara);
    });


    NoticeCenter::Instance().addListener(this,"closeChannel",[mprj](std::string channel){
        TdChanManager *mTag = mprj ;
        printf("closeChannel\r\n");
        mTag->delChannel(channel);
    });

    NoticeCenter::Instance().addListener(this,"createChannel",[mprj](std::string channel){
        TdChanManager *mTag = mprj ;
        printf("createChannel\r\n");
        mTag->createChannel(channel,"192.168.11.126",30252,30254);
    });
    return 0;
}

int TdChanManager::createChannel(std::string channel,std::string dest,int destPort,int basePort)
{
    int status = -1 ;
    pthread_mutex_lock(&chanMapMutex);
    if(chanMap[channel])
    {
        pthread_mutex_unlock(&chanMapMutex);
        spdlog::error("通道({}) 已存在",channel);
        return status ;
    } 
    pthread_mutex_unlock(&chanMapMutex);
  
    TdRtp *gRtp = new TdRtp();
    status = gRtp->setUp(channel,dest.c_str(),destPort,basePort);
    if(status<0)
    {
        delete gRtp ;
        spdlog::error("创建通道失败 原因:{}",status);
        return status ;
    }
    gRtp->start(1);
    pthread_mutex_lock(&chanMapMutex);
    chanMap[channel] = gRtp;
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

