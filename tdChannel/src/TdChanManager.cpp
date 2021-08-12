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
    Channel *ch = mTag->chanMap[channel];
    if(ch==nullptr)
        return  ;
    CodecParam lCodecpara;
    memset(&lCodecpara,0,sizeof(CodecParam));
    spdlog::info("修改视频参数为-> 帧率:{} 分辨率:{} 最大比特:{}",Rate,Resolution,MaxBitrate);
    
    Resolution_t lRes(Resolution);

    lCodecpara.height = lRes.mHeight;
    lCodecpara.width = lRes.mWidth;
    lCodecpara.rate = Rate;
    lCodecpara.maxBitrate = MaxBitrate;
   
    ch->mFifos.OnceWrite(ModifyCodecParam,(char*)&lCodecpara,sizeof(CodecParam));
    ch->mCMediaParam.mRes.mHeight=lCodecpara.height;
    ch->mCMediaParam.mRes.mWidth = lCodecpara.width = lRes.mWidth;
    ch->mCMediaParam.Fps = lCodecpara.rate;
    ch->mCMediaParam.MaxBitrate = lCodecpara.maxBitrate;
    });


    NoticeCenter::Instance().addListener(this,"closeChannel",[mprj](std::string channel){
        TdChanManager *mTag = mprj ;
        printf("closeChannel\r\n");
        mTag->delChannel(channel);
    });

    NoticeCenter::Instance().addListener(this,"createChannel",[mprj](std::string channel){
        TdChanManager *mTag = mprj ;
        printf("createChannel\r\n");
        //mTag->createChannel(channel,"192.168.11.126",30252,30254);
    });
    return 0;
}

int TdChanManager::createChannel(std::string channel,std::string dest,int destPort,int basePort,ChanMediaParam_t param)
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
    Channel *lChannel;
    pid_t fpid;
	fpid = fork();
    if(fpid < 0){
        perror("fork failed");
    }
    if(fpid == 0){
        TdRtp *gRtp = new TdRtp();
        status = gRtp->setUp(channel,dest.c_str(),destPort,basePort);
        if(status<0)
        {
            delete gRtp ;
            spdlog::error("创建通道失败 原因:{}",status);
            return status ;
        }
        CodecParam lCodecpara;
        lCodecpara.height = param.mRes.mHeight;
        lCodecpara.width = param.mRes.mWidth;
        lCodecpara.rate = param.Fps;
        lCodecpara.maxBitrate = param.MaxBitrate;
        gRtp->setParam(lCodecpara);
        gRtp->start(100);
    }else{
        lChannel = new Channel();
        lChannel->mFifos.Create(channel);
        lChannel->pid = fpid;
        lChannel->chanName = channel;
        lChannel->mCMediaParam = param ;
		pthread_mutex_lock(&chanMapMutex);
        chanMap[channel] = lChannel;
        pthread_mutex_unlock(&chanMapMutex);
	}
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
            Channel *lChannel =nullptr;
            pthread_mutex_lock(&chanMannager->chanMapMutex);
            lChannel = chanMannager->chanMap[chanId];
            if(lChannel!=nullptr)
            {
                chanMannager->chanMap.erase(chanId);
            }
            pthread_mutex_unlock(&chanMannager->chanMapMutex);
            if(lChannel!=nullptr)
            {
                kill(lChannel->pid,SIGTERM);
                //printf("wait ...\r\n");
                if(waitpid(lChannel->pid,nullptr,0)==lChannel->pid)
                {
                    unlink(lChannel->mFifos.GetFifoName().c_str());
                    lChannel->mFifos.Close();
                    delete lChannel;
                }
                //printf("ok\r\n");
            }
        }
        
        usleep(3000);
    }
}

std::string TdChanManager::channelsInfo2Json()
{
    json11::Json::array lChannels;
    lChannels.clear();
    Channel* chan =nullptr;
    int channelsTotal = 0 ;
    //json11::Json channelInfo;
    pthread_mutex_lock(&chanMapMutex);
    channelsTotal = chanMap.size();
    std::map<std::string ,Channel*>::iterator iter;
    for (iter=chanMap.begin(); iter!=chanMap.end(); iter++)
    {
        chan = iter->second;
        json11::Json channelInfo = json11::Json::object{
                {"chanID",  chan->chanName },
                {"chanPID",   chan->pid},
                {"height",   chan->mCMediaParam.mRes.mHeight},
                {"width",   chan->mCMediaParam.mRes.mWidth},
                {"Fps",    chan->mCMediaParam.Fps},
        };
        lChannels.push_back(channelInfo);
    }
    pthread_mutex_unlock(&chanMapMutex);
    json11::Json channelsJson = json11::Json::object{
                {"total",  channelsTotal },
                {"chanInfo",   lChannels},
        };
    //printf("channelsJson:%s\r\n",channelsJson.dump().c_str());
    std::string ret = channelsJson.dump();
    return ret;
}

int TdChanManager::mediaPreset(std::string chan,ChanMediaParam_t param)
{
    if(chan.empty())
        return -1 ;
    Channel *lChannel = nullptr;
    CodecParam lCodecpara;
    memset(&lCodecpara,0,sizeof(CodecParam));
    pthread_mutex_lock(&chanMapMutex);
    lChannel = chanMap[chan];
    if(lChannel==nullptr)
    {
        return -1 ;
    }
    pthread_mutex_unlock(&chanMapMutex);
    if(lChannel!=nullptr)
    {
        lCodecpara.height = param.mRes.mHeight;
        lCodecpara.width = param.mRes.mWidth;
        lCodecpara.rate = param.Fps;
        lCodecpara.maxBitrate = param.MaxBitrate;

        lChannel->mFifos.OnceWrite(ModifyCodecParam,(char*)&lCodecpara,sizeof(CodecParam));
        lChannel->mCMediaParam = param;
        printf("%d %d\r\n",lChannel->mCMediaParam.mRes.mHeight,lChannel->mCMediaParam.mRes.mWidth);
    }
    return 0 ;
}
