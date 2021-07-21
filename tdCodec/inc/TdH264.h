#ifndef _TD_H264_H_
#define _TD_H264_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "codec_api.h"
#include "limits.h"
#include "codec_def.h"
#include "codec_app_def.h"
#include "inc/TdScale.h"
#include <fstream>
#include <stdio.h>
#include <queue>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "spdlog/spdlog.h"

typedef struct  {
    uint8_t * data[3];
    int stride[2];
    int width;
    int height;
}Frame_t;

typedef struct FramePacket_s{
    FramePacket_s(uint8_t * Data,uint32_t Size):data(Data),size(Size){};
    uint8_t * data;
    uint32_t size;
}FramePacket;

class TdH264:public TdScale 
{
private:
    ISVCDecoder* decoder_;
    ISVCEncoder* encoder_;
    SDecodingParam decParam;
    SEncParamExt encParam;
    Source_Picture_s picture_;
    uint64_t timestamp_;
    bool mIsEncoderSetUp = false;
    std::queue<FramePacket*> frameQue;
    pthread_mutex_t queMutex;
    bool loopThrStatus = 0 ;
    bool frameQueStatus = true;
    static void loopThread(TdH264 *h264);
    std::thread th;
    //uint8_t* picture_buffer_;
    /* data */
public:
    TdH264(/* args */)
    {
        pthread_mutex_init(&queMutex,NULL);
        th = std::thread(loopThread,this);
        //t.detach();
    }
    std::ofstream *rtpfout ;
    std::ofstream *h264fout ;
    std::ofstream *yuvScalefout ;
    std::ofstream *yuvfout ;
    
    int32_t decoderSetUp();
    int32_t encoderSetUp();
    FramePacket* newFramePacket(uint8_t*data,uint32_t size);
    void DelFramePacket(FramePacket*frame);
    FramePacket* popFrameFromQue();
    int32_t pushFrameToQue(FramePacket * frame);
    bool isEncoderSetUp(){return mIsEncoderSetUp;}
    int32_t setEncoderParam(int width,int height,int bitrate,int iMaxBitrate,int fps);
    void setPicture(int width,int height,int Ystride,int UVstride);
    int32_t decode(uint8_t * inData,uint32_t inSize);
    int32_t encode(uint8_t* Ydata,uint8_t* Udata,uint8_t* Vdata,uint8_t* pkt,size_t& pkt_size,bool& is_keyframe,bool& got_output);
    int32_t destory();
    virtual void h264CallBackUser(uint8_t * data,int size )=0;
    ~TdH264(){
        pthread_mutex_destroy(&queMutex);
        loopThrStatus = 0;
    };
};

#endif