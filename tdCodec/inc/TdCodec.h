#ifndef _TD_CODEC_H_
#define _TD_CODEC_H_ 

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "TdH264.h"

using namespace std::placeholders;
using namespace toolkit;
// typedef struct  {
//     uint8_t * data[3];
//     int stride[2];
//     int width;
//     int height;
// }Frame_t;

//typedef void (*h264CallBackUser)(void * user,uint8_t * data,int size )

// typedef struct FramePacket_s{
//     FramePacket_s(uint8_t * Data,uint32_t Size):data(Data),size(Size){};
//     uint8_t * data;
//     uint32_t size;
// }FramePacket;

typedef struct {
    int width;
    int height;
    int rate;
    int maxBitrate;
}CodecParam ;

enum CodecType{
    VideoH264,
    VideoH265,
};

class TdCodec
{
private:
    void * mCodec;
    CodecType mCodecType;
    /* data */
public:
    TdCodec(/* args */){};
    ~TdCodec(){printf("%s\r\n",__func__);};
    int32_t init(CodecType type);
    int32_t setParam(CodecParam param);
    int32_t convert(uint8_t *inDate,uint32_t inSize,uint8_t **outDate,uint32_t *outSize,bool &isKeyFrame);
    int32_t destroy();
};


#endif