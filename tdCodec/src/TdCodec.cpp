#include "TdCodec.h"

int32_t TdCodec::init(CodecType type)
{
    if(type==VideoH264){
        mCodec = (void*)new TdH264();
        TdH264 * codecH264 = (TdH264 *)mCodec;
        mCodecType = VideoH264 ;
        codecH264->create();
        return 0 ;
    }
    return -1;
}
int32_t TdCodec::setParam(CodecParam param)
{
     if(mCodecType==VideoH264){
        TdH264 * CodecH264 = (TdH264 *)mCodec;
        CodecH264->setCodecHeight(param.height);
        CodecH264->setCodecWidth(param.width);
        if(param.rate<15)
        {
            CodecH264->setCodecRate(param.rate);
        }
        if(param.maxBitrate > CodecH264->getCodecBaseBitrate())
        {
            CodecH264->setCodecMaxBitrate(param.maxBitrate);
        }
        CodecH264->setCodecParamRefresh(true);
        return 0 ;
    }
    return -1 ;
}
int32_t TdCodec::convert(uint8_t *inDate,uint32_t inSize,uint8_t **outDate,uint32_t *outSize,bool &isKeyFrame)
{
     if(mCodecType==VideoH264){
        TdH264 * CodecH264 = (TdH264 *)mCodec;
        CodecH264->convert(inDate,inSize,outDate,outSize,isKeyFrame);
        return 0 ;
    }
    return -1 ;
}
int32_t TdCodec::destroy()
{
    if(mCodecType==VideoH264){
        TdH264 * CodecH264 = (TdH264 *)mCodec;
        CodecH264->destory();
        delete CodecH264 ;
        return 0 ;
    }
    return -1 ;
}