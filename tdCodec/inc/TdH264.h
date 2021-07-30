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
// #include "spdlog/spdlog.h"
#include "inc/noticeCenter.h"
#include "inc/FunHandler.h"
#include "inc/TdCodecParam.h"
// #ifdef DMALLOC
// #include "dmalloc.h"
// #endif


using namespace toolkit;


class TdH264:public TdScale ,public TdCodecParam
{
private:
    ISVCDecoder* decoder_;
    ISVCEncoder* encoder_;
    SDecodingParam decParam;
    SEncParamExt encParam;
    Source_Picture_s picture_;
    uint64_t timestamp_;
    /* data */
public:
    TdH264(/* args */){};
    ~TdH264(){};
    std::ofstream *rtpfout ;
    std::ofstream *h264fout ;
    std::ofstream *yuvScalefout ;
    std::ofstream *yuvfout ;
    FunHandler mFunHandler ;
    int32_t decoderSetUp();
    int32_t encoderSetUp();
    int32_t setEncoderParam(int width,int height,int bitrate,int iMaxBitrate,int fps);
    void setPicture(int width,int height,int Ystride,int UVstride);
    int32_t decode(uint8_t * inData,uint32_t inSize);
    int32_t encode(uint8_t* Ydata,uint8_t* Udata,uint8_t* Vdata,uint8_t** pkt,uint32_t* pkt_size,bool& is_keyframe,bool& got_output);
    int32_t convert(uint8_t *inDate,uint32_t inSize,uint8_t **outDate,uint32_t *outSize,bool &isKeyFrame);
    int32_t destory();
    int32_t create();
    //void setCodecParam(TdCodecParam * para);
};

#endif