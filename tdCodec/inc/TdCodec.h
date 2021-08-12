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

typedef struct Resolution_{
    Resolution_(std::string ResolutionStr)
    {
        mResolutionStr = ResolutionStr ;
        if(!mResolutionStr.compare("1080p"))
        {
            mHeight = 1080 ;
            mWidth = 1920;
        }
        else if(!mResolutionStr.compare("720p"))
        {
            mHeight = 720 ;
            mWidth = 1280;
        }
        else if(!mResolutionStr.compare("qHD"))
        {
            mHeight = 540 ;
            mWidth = 960;
        }
        else if(!mResolutionStr.compare("nHD"))
        {
            mHeight = 360 ;
            mWidth =  640;
        }
        else if(!mResolutionStr.compare("WQVGA"))
        {
            mHeight = 272 ;
            mWidth =  480;
        }
        else if(!mResolutionStr.compare("FWQVGA"))
        {
            mHeight = 240 ;
            mWidth =  432;
        }
        else
        {
            mHeight = 540 ;
            mWidth = 960;
        }
    }
    std::string mResolutionStr ;
    int mWidth = 960;
    int mHeight = 540;
} Resolution_t ;

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
    // Resolution_t parseResolution(std::string Resolution);
};


#endif