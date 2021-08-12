#ifndef _TD_X264_H_
#define _TD_X264_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "limits.h"
#include <fstream>
#include <stdio.h>
#include <queue>
#include <thread>
#include <mutex>
#include <unistd.h>
namespace Td{

#include <x264.h>

typedef struct{
    int width;
    int height;
    int Ystride;
    int UVstride;
}Picture_t;

//class TdX264Handle:public TdScale ,public TdCodecParam
class TdX264Handle
{
private:
    x264_param_t encParam;
    x264_t * encoder_;
    x264_picture_t picture_;
    uint64_t timestamp_;
    Picture_t picParam;
    /* data */
public:
    TdX264Handle(/* args */){};
    ~TdX264Handle(){};
    std::ofstream *rtpfout ;
    std::ofstream *h264fout ;
    std::ofstream *yuvScalefout ;
    std::ofstream *yuvfout ;
    int32_t encoderSetUp();
    int32_t setEncoderParam(int width,int height,int bitrate,int iMaxBitrate,int fps);
    void setPicture(int width,int height,int Ystride,int UVstride);
    int32_t encode(uint8_t* Ydata,uint8_t* Udata,uint8_t* Vdata,uint8_t** pkt,uint32_t* pkt_size,bool& is_keyframe,bool& got_output);
    int32_t convert(uint8_t *inDate,uint32_t inSize,uint8_t **outDate,uint32_t *outSize,bool &isKeyFrame);
    int32_t destory();
    int32_t create();
};

}

#endif